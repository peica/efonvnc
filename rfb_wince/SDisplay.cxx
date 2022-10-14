/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

// -=- SDisplay.cxx
//
// The SDisplay class encapsulates a particular system display.

#include <rfb_wince/SDisplay.h>
#include <rfb_wince/SDisplayCorePolling.h>
#include <rfb/Exception.h>
#include <rfb/LogWriter.h>


using namespace rdr;
using namespace rfb;
using namespace rfb::win32;

static LogWriter vlog("SDisplay");

// - SDisplay-specific configuration options


//////////////////////////////////////////////////////////////////////////////
//
// SDisplay
//

// -=- Constructor/Destructor

SDisplay::SDisplay()
: server(0), pb(0), device(0),
core(0),ptr(0), kbd(0),
statusLocation(0)
{
  updateEvent.h = CreateEvent(0, TRUE, FALSE, 0);
}

SDisplay::~SDisplay()
{
  // XXX when the VNCServer has been deleted with clients active, stop()
  // doesn't get called - this ought to be fixed in VNCServerST.  In any event,
  // we should never call any methods on VNCServer once we're being deleted.
  // This is because it is supposed to be guaranteed that the SDesktop exists
  // throughout the lifetime of the VNCServer.  So if we're being deleted, then
  // the VNCServer ought not to exist and therefore we shouldn't invoke any
  // methods on it.  Setting server to zero here ensures that stop() doesn't
  // call setPixelBuffer(0) on the server.
  server = 0;

  if (core) stop();
}


// -=- SDesktop interface

void SDisplay::start(VNCServer* vs)
{
	vlog.debug("starting");
	
	// Start the SDisplay core
	server = vs;
	startCore();
	
	vlog.debug("started");
	
	if (statusLocation) *statusLocation = true;
}

void SDisplay::stop()
{
	vlog.debug("stopping");
	
	// Stop the SDisplayCore
	if (server)
		server->setPixelBuffer(0);
	stopCore();
	server = 0;
	
	vlog.debug("stopped");
	
	if (statusLocation) *statusLocation = false;
}


void SDisplay::startCore() {
	
	// Initialise the change tracker and clipper
	updates.clear();
	clipper.setUpdateTracker(server);

	// Create the framebuffer object
	recreatePixelBuffer(true);
	
	// Create the SDisplayCore
	try {
		// Note:
		// This time we just provide polling update method
		core = new SDisplayCorePolling(this, &updates, 240);	
		core->setScreenRect(screenRect);
	} catch (rdr::Exception& e) {
		if (core) {
			delete core; 
			core = 0;
		}
		vlog.error(e.str());
		throw rdr::Exception("unable to access desktop");		
	}

	vlog.info("Started %s", core->methodName());

	// Start input handlers
	ptr = new SPointer;
	kbd = new SKeyboard;
}

void SDisplay::stopCore() {
	if (core)
		vlog.info("Stopping %s", core->methodName());
	delete core; core = 0;
	delete pb; pb = 0;
	delete device; device = 0;
	delete ptr; ptr = 0;
	delete kbd; kbd = 0;
	ResetEvent(updateEvent);
}


bool SDisplay::areHooksAvailable() {
	return false;
}

bool SDisplay::isDriverAvailable() {
	return false;
}


bool SDisplay::isRestartRequired() {
	// - We must restart the SDesktop if:
	// 1. We are no longer in the input desktop.
	// 2. The any setting has changed.
	return false;
}


void SDisplay::restartCore() {
  vlog.info("restarting");

  // Stop the existing Core  related resources
  stopCore();
  try {
    // Start a new Core if possible
    startCore();
    vlog.info("restarted");
  } catch (rdr::Exception& e) {
    // If startCore() fails then we MUST disconnect all clients,
    // to cause the server to stop() the desktop.
    // Otherwise, the SDesktop is in an inconsistent state
    // and the server will crash.
    server->closeClients(e.str());
  }
}


void SDisplay::pointerEvent(const Point& pos, int buttonmask) {
  if (pb->getRect().contains(pos)) {
    Point screenPos = pos.translate(screenRect.tl);
    // - Check that the SDesktop doesn't need restarting
    if (isRestartRequired())
      restartCore();
    if (ptr)
      ptr->pointerEvent(screenPos, buttonmask);
  }
}

void SDisplay::keyEvent(rdr::U32 key, bool down) {
  // - Check that the SDesktop doesn't need restarting
  if (isRestartRequired())
    restartCore();
  if (kbd)
    kbd->keyEvent(key, down);
}

void SDisplay::clientCutText(const char* text, int len) {
}


void SDisplay::framebufferUpdateRequest()
{
  SetEvent(updateEvent);
}

Point SDisplay::getFbSize() {
  bool startAndStop = !core;

  // If not started, do minimal initialisation to get desktop size.
  if (startAndStop)
    recreatePixelBuffer();

  Point result = Point(pb->width(), pb->height());

  // Destroy the initialised structures.
  if (startAndStop)
    stopCore();
  return result;
}

void
SDisplay::processEvent(HANDLE event) {
	if (event == updateEvent) {
		//vlog.write(120, "processEvent");
		ResetEvent(updateEvent);

		// - If the SDisplay isn't even started then quit now
		if (!core) {
			vlog.error("not start()ed");
			return;
		}
		
		// - Only process updates if the server is ready
		if (server && server->clientsReadyForUpdate()) {
			
			// - Check that the SDesktop doesn't need restarting
			if (isRestartRequired()) {
				restartCore();
				return;
			}
			
			// - Flush any updates from the core
			try {
				core->flushUpdates();
			} catch (rdr::Exception& e) {
				vlog.error(e.str());
				restartCore();
				return;
			}
			
			// Flush any changes to the server
			if (flushChangeTracker()) {
				server->tryUpdate();
			}
		}
		return;
	}
	throw rdr::Exception("No such event");
}


// -=- Protected methods

void
SDisplay::recreatePixelBuffer(bool force) {
	// Open the specified display device
	
	DeviceContext* new_device = 0;
	
	vlog.info("Attaching to virtual desktop");
	new_device = new WindowDC(0);
	
	// Get the coordinates of the specified dispay device
	Rect newScreenRect = new_device->getClipBox();
	
	// If nothing has changed & a recreate has not been forced, delete
	// the new device context and return
	if (pb && !force &&
		newScreenRect.equals(screenRect) &&
		new_device->getPF().equal(pb->getPF())) {
		delete new_device;
		return;
	}
	
	// Flush any existing changes to the server
	flushChangeTracker();
	
	// Delete the old pixelbuffer and device context
	vlog.debug("deleting old pixel buffer & device");
	if (pb)
		delete pb;
	if (device)
		delete device;
	
	// Create a DeviceFrameBuffer attached to the new device
	vlog.debug("creating pixel buffer");
	DeviceFrameBuffer* new_buffer = new DeviceFrameBuffer(*new_device);
	
	// Replace the old PixelBuffer
	screenRect = newScreenRect;
	pb = new_buffer;
	device = new_device;
	
	// Initialise the pixels
	pb->grabRegion(pb->getRect());
	
	// Prevent future grabRect operations from throwing exceptions
	// pb->setIgnoreGrabErrors(true);
	pb->setIgnoreGrabErrors(false);
	
	// Update the clipping update tracker
	clipper.setClipRect(pb->getRect());
	
	// Inform the core of the changes
	if (core)
		core->setScreenRect(screenRect);

	// Inform the server of the changes
	if (server)
		server->setPixelBuffer(pb);
}

bool SDisplay::flushChangeTracker() {
	if (updates.is_empty())
		return false;
	
	// vlog.write(120, "flushChangeTracker");
	
	// Translate the update coordinates from Screen coords to Desktop
	updates.translate(screenRect.tl.negate());
	
	// Clip the updates & flush them to the server
	updates.copyTo(&clipper);
	updates.clear();
	return true;
}

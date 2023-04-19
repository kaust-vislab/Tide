/*********************************************************************/
/* Copyright (c) 2018, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE  IS  PROVIDED  BY  THE  ECOLE  POLYTECHNIQUE    */
/*    FEDERALE DE LAUSANNE  ''AS IS''  AND ANY EXPRESS OR IMPLIED    */
/*    WARRANTIES, INCLUDING, BUT  NOT  LIMITED  TO,  THE  IMPLIED    */
/*    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR  A PARTICULAR    */
/*    PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL  THE  ECOLE    */
/*    POLYTECHNIQUE  FEDERALE  DE  LAUSANNE  OR  CONTRIBUTORS  BE    */
/*    LIABLE  FOR  ANY  DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,    */
/*    EXEMPLARY,  OR  CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT    */
/*    LIMITED TO,  PROCUREMENT  OF  SUBSTITUTE GOODS OR SERVICES;    */
/*    LOSS OF USE, DATA, OR  PROFITS;  OR  BUSINESS INTERRUPTION)    */
/*    HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,  WHETHER IN    */
/*    CONTRACT, STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE    */
/*    OR OTHERWISE) ARISING  IN ANY WAY  OUT OF  THE USE OF  THIS    */
/*    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of Ecole polytechnique federale de Lausanne.          */
/*********************************************************************/

#include "WindowTouchController.h"

#include "control/ContentController.h"
#include "control/DisplayGroupController.h"
#include "scene/DisplayGroup.h"

WindowTouchController::WindowTouchController(Window& window,
                                             DisplayGroup& group)
    : _window{window}
    , _group{group}
    , _controller{window, group}
{
}

void WindowTouchController::onTouchStarted()
{
    DisplayGroupController{_group}.moveWindowToFront(_window.getID());
}

void WindowTouchController::onTap()
{
    //    print_log(LOG_INFO, LOG_REST, "Tap on touch controller");
    if (_isWindowActive() && !_window.isPanel())
        _controller.toggleSelected();
}

void WindowTouchController::onTapAndHold()
{
    //    print_log(LOG_INFO, LOG_REST, "Tap and hold touch controller");
    if (!_window.isFullscreen() && !_window.isFocused())
    {
        _window.getContent().setCaptureInteraction(
            !_window.getContent().getCaptureInteraction());
    }

    if (_window.isPanel())
        _controller.toggleSelected(); // force toggle
}

// Edited so that it exits full screen mode on a double tap.
// Doing the max full screen function seemed useless, and exiting
// full screen was difficult without brining up the control screen
void WindowTouchController::onDoubleTap(const uint numPoints)
{
    //    print_log(LOG_INFO, LOG_REST, "Double tap on touch controller");
    //    if (_window.isFullscreen())
    //        _controller.toogleFullscreenMaxSize();
    //    else
    {
        if (numPoints > 1)
            _toggleFocusMode();
        else
            DisplayGroupController{_group}.toggleFullscreen(_window.getID());
    }
}

void WindowTouchController::onPanStarted()
{
    if (_isWindowActive() && _window.isIdle())
        _window.setState(Window::WindowState::MOVING);
}

void WindowTouchController::onPan(const QPointF& /*pos*/, const QPointF& delta,
                                  const uint numPoints)
{
    if (_isWindowActive() && _window.isMoving() && numPoints == 1)
        _controller.moveBy(delta);
}

void WindowTouchController::onPanEnded()
{
    if (_isWindowActive() && _window.isMoving())
        _window.setState(Window::WindowState::NONE);
}

void WindowTouchController::onPinchStarted()
{
    if (_isWindowActive() && _window.isIdle())
        _window.setState(Window::WindowState::RESIZING);
}

void WindowTouchController::onPinch(const QPointF& pos, const QPointF& delta)
{
    if (_isWindowActive() && _window.isResizing())
        _controller.scale(pos, delta);
}

void WindowTouchController::onPinchEnded()
{
    if (_isWindowActive() && _window.isResizing())
        _window.setState(Window::WindowState::NONE);
}

void WindowTouchController::_toggleFocusMode()
{
    if (_window.isFocused())
        DisplayGroupController{_group}.unfocus(_window.getID());
    else
        DisplayGroupController{_group}.focusSelected();
}

bool WindowTouchController::_isWindowActive() const
{
    return !_window.isFocused();
}

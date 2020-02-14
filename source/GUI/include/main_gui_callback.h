#pragma once

#include <CEGUI/CEGUI.h>

void buttonClickCallback(const CEGUI::EventArgs& args);

void sliderMoveCallback(const CEGUI::EventArgs& args);

void frameWindowCloseCallback(const CEGUI::EventArgs& args);

void mainGUICallback(const CEGUI::EventArgs &args, int type);

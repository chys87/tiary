#include "ui/button_default.h"
#include "ui/button.h"


namespace tiary {
namespace ui {





ButtonDefault::ButtonDefault ()
	: Window ()
	, default_default (0)
{
	Signal sig_tmp (this, &ButtonDefault::slot_default_button);
	Window::register_hotkey (RETURN, sig_tmp, Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
	Window::register_hotkey (NEWLINE, TIARY_STD_MOVE (sig_tmp),
			Hotkeys::CASE_SENSITIVE|Hotkeys::DISALLOW_ALT);
}

ButtonDefault::~ButtonDefault ()
{
}

void ButtonDefault::on_focus_changed ()
{
	const ControlList &control_list = get_control_list ();
	for (ControlList::const_iterator it = control_list.begin ();
			it != control_list.end (); ++it) {
		if (Button *btn = dynamic_cast <Button *> (*it)) {
			btn->Button::redraw ();
		}
	}
}

void ButtonDefault::set_default_button (Button *btn)
{
	default_default = btn;
}

Button *ButtonDefault::get_current_default_button () const
{
	Control *focus = Window::get_focus ();
	if (Button *focus_button = dynamic_cast <Button *> (focus)) {
		return focus_button;
	}
	return default_default;
}

void ButtonDefault::slot_default_button ()
{
	if (Button *btn = get_current_default_button ()) {
		btn->sig_clicked.emit ();
	}
}



ButtonDefaultExtended::ButtonDefaultExtended ()
	: ButtonDefault ()
	, special_map ()
{
}

ButtonDefaultExtended::~ButtonDefaultExtended ()
{
}

void ButtonDefaultExtended::set_special_default_button (Control *focus, Button *btn)
{
	special_map.insert (std::make_pair (focus, btn));
}

Button *ButtonDefaultExtended::get_current_default_button () const
{
	Control *focus = Window::get_focus ();
	if (focus == 0) {
		return default_default;
	}
	if (Button *focus_button = dynamic_cast <Button *> (focus)) {
		return focus_button;
	}
	SpecialMap::const_iterator it = special_map.find (focus);
	if (it == special_map.end ()) {
		return default_default;
	}
	else {
		return it->second;
	}
}

} // namespace tiary::ui
} // namespace tiary

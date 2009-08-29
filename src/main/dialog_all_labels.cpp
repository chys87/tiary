#include "main/dialog_all_labels.h"
#include "ui/button.h"
#include "ui/button_default.h"
#include "ui/dialog.h"
#include "ui/listbox.h"
#include "ui/fixed_dialog.h"
#include "ui/label.h"
#include "ui/layout.h"
#include "ui/chain.h"
#include "ui/dialog_input.h"
#include "ui/dialog_message.h"
#include "diary/diary.h"
#include "common/format.h"
#include "common/containers.h"

namespace tiary {


namespace {

using namespace ui;

class DialogAllLabels : public virtual Dialog, public FixedDialog, private ButtonDefault
{

	ListBox lst_labels;

	Button btn_rename;
	Button btn_delete;
	Button btn_ok;

	Layout layout_main;
	Layout layout_right;

	DiaryEntryList &entries;

	WStringLocaleOrderedSet all_labels;

	bool modified;

public:
	DialogAllLabels (DiaryEntryList &);
	~DialogAllLabels ();

	void redraw ();
	void on_winch ();

	void slot_rename ();
	void slot_delete ();
	void slot_ok ();

	bool get_modified () const { return modified; }

	void refresh_list (const std::wstring &select_hint = std::wstring ());
};

DialogAllLabels::DialogAllLabels (DiaryEntryList &entries_)
	: Dialog (0, L"All labels")
	, FixedDialog ()
	, ButtonDefault ()
	, lst_labels (*this)
	, btn_rename (*this, L"&Rename")
	, btn_delete (*this, L"&Delete")
	, btn_ok (*this, L"&OK")
	, layout_main (HORIZONTAL)
	, layout_right (VERTICAL)
	, entries (entries_)
	, all_labels ()
	, modified (false)
{
	for (DiaryEntryList::const_iterator it = entries.begin (); it != entries.end (); ++it)
		all_labels.insert ((*it)->labels.begin (), (*it)->labels.end ());
	refresh_list ();

	layout_right.add
		(btn_rename, 3, 3)
		(1, 1)
		(btn_delete, 3, 3)
		(1, Layout::UNLIMITED)
		(btn_ok, 3, 3)
		;
	layout_main.add
		(lst_labels, 1, Layout::UNLIMITED)
		(1, 1)
		(layout_right, 10, 10)
		;

	btn_rename.sig_clicked.connect (this, &DialogAllLabels::slot_rename);
	btn_delete.sig_clicked.connect (this, &DialogAllLabels::slot_delete);
	btn_ok.sig_clicked.connect (this, &DialogAllLabels::slot_ok);

	lst_labels.register_hotkey (DELETE, btn_delete.sig_clicked);

	register_hotkey (ESCAPE, btn_ok.sig_clicked);
	set_default_button (btn_ok);

	DialogAllLabels::redraw ();
}

DialogAllLabels::~DialogAllLabels ()
{
}

void DialogAllLabels::redraw ()
{
	Size size = get_screen_size () & make_size (40, 40);
	FixedDialog::resize (size);
	layout_main.move_resize (make_size (2, 1), size - make_size (4, 2));
	Dialog::redraw ();
}

void DialogAllLabels::on_winch ()
{
	DialogAllLabels::redraw ();
}

void DialogAllLabels::slot_rename ()
{
	size_t k = lst_labels.get_select ();
	if (k < lst_labels.get_items ().size ()) {
		const std::wstring &old_name = lst_labels.get_items () [k];
		std::wstring new_name = dialog_input2 (
				L"Rename label",
				format (L"Enter the new name for \"%a\":") << old_name,
				old_name,
				32);
		if (new_name.empty () || new_name == old_name)
			return;

		const wchar_t *warning_template;

		if (all_labels.find (new_name) != all_labels.end ()) // This label already exists
			warning_template = L"Label \"%b\" already exists. Are you sure you want to merge \"%a\" into \"%b\"?\n"
						L"This operation cannot be undone!";
		else
			warning_template = L"Are you sure you want to rename \"%a\" to \"%b\"?";

		if (dialog_message (format (warning_template) << old_name << new_name,
					L"Rename label", MESSAGE_YES|MESSAGE_NO) == MESSAGE_YES) {

			all_labels.erase (old_name);
			all_labels.insert (new_name);

			for (DiaryEntryList::iterator it = entries.begin (); it != entries.end (); ++it) {
				DiaryEntry::LabelList &labels = (*it)->labels;
				DiaryEntry::LabelList::iterator jt = labels.find (old_name);
				if (jt != labels.end ()) {
					labels.erase (jt);
					labels.insert (new_name);
				}
			}
			modified = true;
			refresh_list (new_name);
		}
	}
}

void DialogAllLabels::slot_delete ()
{
	size_t k = lst_labels.get_select ();
	if (k < lst_labels.get_items ().size ()) {
		const std::wstring &old_name = lst_labels.get_items () [k];
		if (dialog_message (format (L"Are you sure you want to delete label \"%a\"?\nThis operation cannot be undone!")
					<< old_name, L"Delete label", MESSAGE_YES|MESSAGE_NO) == MESSAGE_YES) {
			all_labels.erase (old_name);
			for (DiaryEntryList::iterator it = entries.begin (); it != entries.end (); ++it)
				(*it)->labels.erase (old_name);
			modified = true;
			refresh_list ();
		}
	}
}

void DialogAllLabels::slot_ok ()
{
	Window::request_close ();
}

void DialogAllLabels::refresh_list (const std::wstring &select_hint)
{
	size_t new_select;
	WStringLocaleOrderedSet::const_iterator it = all_labels.find (select_hint);
	if (it == all_labels.end ())
		new_select = size_t (-1);
	else
		new_select = std::distance (all_labels.begin (), it);
	lst_labels.set_items (std::vector<std::wstring> (all_labels.begin (), all_labels.end ()),
			new_select, false);
}

} // anonymous namespace

bool edit_all_labels (DiaryEntryList &entries)
{
	DialogAllLabels win (entries);
	win.event_loop ();
	return win.get_modified ();
}

} // namespace tiary

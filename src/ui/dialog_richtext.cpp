#include "ui/dialog_richtext.h"
#include "ui/richtext.h"
#include "ui/dialog.h"
#include <utility> // std::forward


namespace tiary {
namespace ui {

namespace {

class DialogRichText : public Dialog
{

	RichText text;
	Size size_hint;
	unsigned max_text_width;

public:
	DialogRichText (const std::wstring &title, const RichTextList &list, Size size_hint);
#ifdef TIARY_HAVE_RVALUE_REFERENCES
	DialogRichText (const std::wstring &title, RichTextList &&list, Size size_hint);
#endif
	void ctor_finish ();
	~DialogRichText ();

	void redraw ();
	void on_winch ();
};

DialogRichText::DialogRichText (const std::wstring &title, const RichTextList &lst, Size size_hint_)
	: Dialog (0, title)
	, text (*this, lst)
	, size_hint (size_hint_)
{
	ctor_finish ();
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
DialogRichText::DialogRichText (const std::wstring &title, RichTextList &&lst, Size size_hint_)
	: Dialog (0, title)
	, text (*this, std::forward <RichTextList> (lst))
	, size_hint (size_hint_)
{
	ctor_finish ();
}
#endif

void DialogRichText::ctor_finish ()
{
	// Continue with the c'tor
	max_text_width = 0;
	for (RichTextList::const_iterator it = text.get_list ().begin ();
			it != text.get_list ().end (); ++it)
		if (max_text_width < it->screen_wid)
			max_text_width = it->screen_wid;

	Signal sig_close (this, &Window::request_close);
	register_hotkey (RETURN, sig_close);
	register_hotkey (NEWLINE, sig_close);
	register_hotkey (ESCAPE, sig_close);
	register_hotkey (L'q', TIARY_STD_MOVE (sig_close));

	DialogRichText::redraw ();
}

DialogRichText::~DialogRichText ()
{
}

void DialogRichText::redraw ()
{
	Size ideal_size = size_hint;
	if (ideal_size.x < max_text_width)
		ideal_size.x = max_text_width + 12;
	if (ideal_size.y < text.get_list ().size ())
		ideal_size.y = text.get_list ().size () + 3;
	ideal_size += make_size (4, 2); // Border
	Size scrsize = get_screen_size ();
	ideal_size &= scrsize;

	Dialog::move_resize ((scrsize - ideal_size)/2, ideal_size);
	text.move_resize (make_size (2, 1), ideal_size - make_size (4, 2));
	Dialog::redraw ();
}

void DialogRichText::on_winch ()
{
	DialogRichText::redraw ();
}

} // anonymous namespace

void dialog_richtext (const std::wstring &title, const RichTextList &list,
		Size size_hint)
{
	DialogRichText (title, list, size_hint).event_loop ();
}

#ifdef TIARY_HAVE_RVALUE_REFERENCES
void dialog_richtext (const std::wstring &title, RichTextList &&list,
		Size size_hint)
{
	DialogRichText (title, std::forward <RichTextList> (list), size_hint).event_loop ();
}
#endif

} // namespace tiary::ui
} // namespace tiary


#include "cdcc.h"

#include <stdio.h>

/* Here are all the functions reacting to signals
 * from the UI.
 */


void on_load_bg_img_btn_file_set(GtkFileChooserButton *btn, CDApplication *cd_app)
{
	cd_model_bg_image_set(cd_app, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(btn)));
}


void on_bg_tint_btn_color_set(GtkColorButton *btn, CDApplication *cd_app)
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &color);
	cd_model_bg_tint_set(cd_app, color);
}


void on_title_color_btn_color_set(GtkColorButton *btn, CDApplication *cd_app)
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &color);
	cd_model_title_text_color_set(cd_app, color);
}


void on_title_font_chooser_font_set(GtkFontButton *btn, CDApplication *cd_app)
{
	cd_model_title_font_set(cd_app, gtk_font_chooser_get_font(GTK_FONT_CHOOSER(btn)));
}


void on_title_bg_color_btn_color_set(GtkColorButton *btn, CDApplication *cd_app)
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &color);
	cd_model_title_bg_color_set(cd_app, color);
}


void on_tracks_count_spin_btn_value_changed(GtkSpinButton *btn, CDApplication *cd_app)
{
	cd_model_tracks_count_set(cd_app, gtk_spin_button_get_value_as_int(btn));
}


void on_tracks_color_btn_color_set(GtkColorButton *btn, CDApplication *cd_app)
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &color);
	cd_model_tracks_texts_color_set(cd_app, color);
}


void on_tracks_font_chooser_font_set(GtkFontButton *btn, CDApplication *cd_app)
{
	cd_model_tracks_font_set(cd_app, gtk_font_chooser_get_font(GTK_FONT_CHOOSER(btn)));
}


void on_tracks_bg_color_btn_color_set(GtkColorButton *btn, CDApplication *cd_app)
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(btn), &color);
	cd_model_tracks_bg_color_set(cd_app, color);
}


void on_cd_tracks_spacing_btn_value_changed(GtkSpinButton *btn, CDApplication *cd_app)
{
	cd_model_tracks_spacing_set(cd_app, gtk_spin_button_get_value_as_int(btn));
}


void on_cd_print_btn_clicked(GtkButton *btn, CDApplication *cd_app)
{
	cd_model_start_printing(cd_app);
}


void on_cd_save_btn_clicked(GtkButton *btn, CDApplication *cd_app)
{
	GtkWidget *file_chooser = gtk_file_chooser_dialog_new(
		"Select a saving location",
		GTK_WINDOW(cd_app->cd_view->window),
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE_AS, GTK_RESPONSE_ACCEPT,
		NULL);
	
	if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
		cd_model_save_to_file(cd_app, filename);
		g_free(filename);
	}

	gtk_widget_destroy(file_chooser);
}


void on_cd_open_file_btn_clicked(GtkButton *btn, CDApplication *cd_app)
{
	GtkWidget *file_chooser = gtk_file_chooser_dialog_new(
		"Select a file to open",
		GTK_WINDOW(cd_app->cd_view->window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);
	
	if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
		cd_model_load_from_file(cd_app, filename);
		g_free(filename);
	}

	gtk_widget_destroy(file_chooser);
}


/* This app has an english and a french
 * translation. To tell which one to
 * choose, I use the file glade/language.
 * If it contains a 1, it's french, else
 * it's english.
 */


void on_cd_select_en_btn_clicked(GtkButton *btn, CDApplication *cd_app)
{
	FILE *f = fopen("./glade/language", "w");

	int language = 0;
	fwrite(&language, sizeof(int), 1, f);

	fclose(f);
}


void on_cd_select_fr_btn_clicked(GtkButton *btn, CDApplication *cd_app)
{
	FILE *f = fopen("./glade/language", "w");

	int language = 1;
	fwrite(&language, sizeof(int), 1, f);

	fclose(f);
}
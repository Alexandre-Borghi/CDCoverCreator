#include "cdcc.h"


static void _application_activate(GtkApplication *gtk_app, CDApplication *cd_app);


CDView * cd_view_new()
{
	CDView *cd_view = malloc(sizeof(CDView));

	cd_view->gtk_app = gtk_application_new("fr.hangingbracket.cdcovercreator", G_APPLICATION_FLAGS_NONE);

	return cd_view;
}


void cd_view_application_activate(CDView *cd_view, CDApplication *cd_app)
{
	g_signal_connect(cd_view->gtk_app, "activate", G_CALLBACK(_application_activate), cd_app);
}


int cd_view_run(CDView *cd_view, int *argc, char ***argv)
{
	int status = g_application_run(G_APPLICATION(cd_view->gtk_app), *argc, *argv);
	return status;
}


void cd_view_destroy(CDView *cd_view)
{
	g_object_unref(cd_view->gtk_app);
}


static void _application_activate(GtkApplication *gtk_app, CDApplication *cd_app)
{
	/* I first open the file in append mode
	 * to create it if it doesn't exists.
	 */

	FILE *create_file_if_not_exists = fopen("./glade/language", "a");
	fclose(create_file_if_not_exists);

	FILE *f = fopen("./glade/language", "rb");

	int language;

	fread(&language, sizeof(int), 1, f);

	fclose(f);

	/* Here I load the UI depending on the
	 * language read in the glade/language
	 * file.
	 */

	GtkBuilder *builder;

	if (language == 1)
	{
		builder = gtk_builder_new_from_file("./glade/cd_ui_fr.glade");
	}
	else
	{
		builder = gtk_builder_new_from_file("./glade/cd_ui_en.glade");
	}


	cd_app->cd_view->window = GTK_WIDGET(gtk_builder_get_object(builder, "cd_window"));
	g_object_set(cd_app->cd_view->window, "application", cd_app->cd_view->gtk_app, NULL);

	cd_app->cd_view->bg_img = GTK_IMAGE(gtk_builder_get_object(builder, "cd_bg_img"));
	cd_app->cd_view->bg_tint_img = GTK_IMAGE(gtk_builder_get_object(builder, "cd_bg_tint_img"));
	cd_app->cd_view->tracks_grid = GTK_GRID(gtk_builder_get_object(builder, "cd_tracks_grid"));

	cd_app->cd_view->load_bg_img_btn          = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "load_bg_img_btn"));
	cd_app->cd_view->pick_bg_tint_btn         = GTK_COLOR_BUTTON       (gtk_builder_get_object(builder, "bg_tint_btn"));

	cd_app->cd_view->pick_title_color_btn     = GTK_COLOR_BUTTON       (gtk_builder_get_object(builder, "title_color_btn"));
	cd_app->cd_view->pick_title_font_btn      = GTK_FONT_BUTTON        (gtk_builder_get_object(builder, "title_font_chooser"));
	cd_app->cd_view->pick_title_bg_color_btn  = GTK_COLOR_BUTTON       (gtk_builder_get_object(builder, "title_bg_color_btn"));

	cd_app->cd_view->tracks_count_btn         = GTK_SPIN_BUTTON        (gtk_builder_get_object(builder, "tracks_count_spin_btn"));
	cd_app->cd_view->pick_tracks_color_btn    = GTK_COLOR_BUTTON       (gtk_builder_get_object(builder, "tracks_color_btn"));
	cd_app->cd_view->pick_tracks_font_btn     = GTK_FONT_BUTTON        (gtk_builder_get_object(builder, "tracks_font_chooser"));
	cd_app->cd_view->pick_tracks_bg_color_btn = GTK_COLOR_BUTTON       (gtk_builder_get_object(builder, "tracks_bg_color_btn"));
	cd_app->cd_view->tracks_spacing_btn       = GTK_SPIN_BUTTON        (gtk_builder_get_object(builder, "cd_tracks_spacing_btn"));

	cd_app->cd_view->cover_overlay            = GTK_WIDGET             (gtk_builder_get_object(builder, "cd_cover_overlay"));
	
	gtk_builder_connect_signals(builder, cd_app);

	g_object_unref(builder);

	gtk_widget_show(cd_app->cd_view->window);

	cd_model_update_view(cd_app);
}


void cd_view_bg_img_set(CDView *cd_view, GdkPixbuf *pixbuf)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(cd_view->bg_img), pixbuf);
}


void cd_view_bg_tint_img_set(CDView *cd_view, GdkPixbuf *pixbuf)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(cd_view->bg_tint_img), pixbuf);
}
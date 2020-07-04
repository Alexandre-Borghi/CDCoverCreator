#include "cdcc.h"
#include <stdio.h>
#include <gtk/gtk.h>

static unsigned int _gdk_rgba_to_uint(GdkRGBA *gdk_rgba);
static void _cd_model_update_css_provider(CDModel *cd_model);
static void _cd_model_draw_page(GtkPrintOperation *op, GtkPrintContext *context, int page_nr, CDApplication *cd_app);
static void _cd_model_begin_print(GtkPrintOperation *op, GtkPrintContext *context, CDApplication *cd_app);
static void _cd_model_bad_file_dialog(CDApplication *cd_app);


static const char *CSS = "\
.cdtitle {\n\
	font: %spx;\n\
}\n\
\n\
.cdtitle text {\n\
	color: #%06x;\n\
	background-color: #%06x;\n\
}\n\
\n\
.cdtrack {\n\
	font: %spx;\n\
}\n\
\n\
.cdtrack text {\n\
	color: #%06x;\n\
	background-color: #%06x;\n\
}\n\
";


CDModel * cd_model_new()
{
	CDModel *cd_model = malloc(sizeof(CDModel));

	cd_model->bg_image = calloc(1, sizeof(char));
	cd_model->bg_image_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 1, 1);
	gdk_pixbuf_fill(cd_model->bg_image_buf, 0x00000000);
	cd_model->bg_tint = (GdkRGBA){ 0.91, 0.30, 0.24, 0.6 };
	cd_model->bg_tint_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, 512, 512);

	cd_model->tracks_count = 0;

	cd_model->tracks_texts_color = (GdkRGBA){ 0.93, 0.94, 0.95, 1.0 };
	cd_model->tracks_texts_font = malloc(sizeof(char) * 8);
	strcpy(cd_model->tracks_texts_font, "Sans 12");
	cd_model->tracks_bg_color = (GdkRGBA){ 0.17, 0.24, 0.31, 1.0 };
	cd_model->tracks_spacing = 6;

	cd_model->title_text = calloc(1, sizeof(char));
	cd_model->title_text_color = (GdkRGBA){ 0.93, 0.94, 0.95, 1.0 };
	cd_model->title_text_font = malloc(sizeof(char) * 8);
	strcpy(cd_model->title_text_font, "Sans 20");
	cd_model->title_bg_color = (GdkRGBA){ 0.17, 0.24, 0.31, 1.0 };

	return cd_model;
}


void cd_model_activate(CDModel *cd_model)
{
	cd_model->css_provider = gtk_css_provider_new();

	_cd_model_update_css_provider(cd_model);
}


void cd_model_destroy(CDModel *cd_model)
{
	free(cd_model->bg_image);

	free(cd_model->tracks_texts_font);
	free(cd_model->title_text);
	free(cd_model->title_text_font);
}


void cd_model_update_view(CDApplication *cd_app)
{
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(cd_app->cd_view->pick_bg_tint_btn),         &cd_app->cd_model->bg_tint);
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(cd_app->cd_view->pick_title_color_btn),     &cd_app->cd_model->title_text_color);
	gtk_font_chooser_set_font (GTK_FONT_CHOOSER (cd_app->cd_view->pick_title_font_btn),       cd_app->cd_model->title_text_font);
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(cd_app->cd_view->pick_title_bg_color_btn),  &cd_app->cd_model->title_bg_color);
	gtk_spin_button_set_value (                  cd_app->cd_view->tracks_count_btn,           cd_app->cd_model->tracks_count);
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(cd_app->cd_view->pick_tracks_color_btn),    &cd_app->cd_model->tracks_texts_color);
	gtk_font_chooser_set_font (GTK_FONT_CHOOSER (cd_app->cd_view->pick_tracks_font_btn),      cd_app->cd_model->tracks_texts_font);
	gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER(cd_app->cd_view->pick_tracks_bg_color_btn), &cd_app->cd_model->tracks_bg_color);
	gtk_spin_button_set_value (                  cd_app->cd_view->tracks_spacing_btn,         cd_app->cd_model->tracks_spacing);

	gtk_image_set_from_pixbuf(cd_app->cd_view->bg_img, cd_app->cd_model->bg_image_buf);
	cd_model_bg_tint_set(cd_app, cd_app->cd_model->bg_tint);

	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_bg_image_set(CDApplication *cd_app, char *bg_image)
{
	int new_bg_image_len = strlen(bg_image);

	if (strlen(cd_app->cd_model->bg_image) < new_bg_image_len)
	{
		cd_app->cd_model->bg_image = realloc(cd_app->cd_model->bg_image, new_bg_image_len + 1);
	}

	strcpy(cd_app->cd_model->bg_image, bg_image);

	GError **error = NULL;
	cd_app->cd_model->bg_image_buf = gdk_pixbuf_new_from_file_at_size(cd_app->cd_model->bg_image, 512, 512, error);
	cd_app->cd_model->bg_image_buf = gdk_pixbuf_add_alpha(cd_app->cd_model->bg_image_buf, FALSE, 0, 0, 0);

	cd_view_bg_img_set(cd_app->cd_view, cd_app->cd_model->bg_image_buf);
}


void cd_model_bg_tint_set(CDApplication *cd_app, GdkRGBA bg_tint)
{
	cd_app->cd_model->bg_tint = bg_tint;
	gdk_pixbuf_fill(cd_app->cd_model->bg_tint_buf, _gdk_rgba_to_uint(&cd_app->cd_model->bg_tint));
	cd_view_bg_tint_img_set(cd_app->cd_view, GDK_PIXBUF(cd_app->cd_model->bg_tint_buf));
}


void cd_model_title_bg_color_set(CDApplication *cd_app, GdkRGBA color)
{
	cd_app->cd_model->title_bg_color = color;
	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_title_text_color_set(CDApplication *cd_app, GdkRGBA color)
{
	cd_app->cd_model->title_text_color = color;
	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_title_font_set(CDApplication *cd_app, char *font)
{
	int new_font_len = strlen(font);

	if (strlen(cd_app->cd_model->title_text_font) < new_font_len)
	{
		cd_app->cd_model->title_text_font = realloc(cd_app->cd_model->title_text_font, new_font_len + 1);
	}

	strcpy(cd_app->cd_model->title_text_font, font);

	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_tracks_count_set(CDApplication *cd_app, int new_tracks_count)
{
	/* Adding tracks */
	while (new_tracks_count > cd_app->cd_model->tracks_count)
	{
		GtkTextView *new_text_view = GTK_TEXT_VIEW(gtk_text_view_new());

		GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(new_text_view));
		gtk_style_context_add_class(context,"cdtrack");

		gtk_text_view_set_accepts_tab(new_text_view, FALSE);

		int new_text_view_x = cd_app->cd_model->tracks_count % 2;
		int new_text_view_y = cd_app->cd_model->tracks_count / 2 + 1;

		gtk_grid_attach(cd_app->cd_view->tracks_grid, GTK_WIDGET(new_text_view), new_text_view_x, new_text_view_y, 1, 1);

		cd_app->cd_model->tracks_count++;

		gtk_widget_show_all(GTK_WIDGET(cd_app->cd_view->tracks_grid));
	}

	/* Removing tracks */
	while (new_tracks_count < cd_app->cd_model->tracks_count)
	{
		cd_app->cd_model->tracks_count--;

		int text_view_x = cd_app->cd_model->tracks_count % 2;
		int text_view_y = cd_app->cd_model->tracks_count / 2 + 1;

		gtk_widget_destroy(gtk_grid_get_child_at(cd_app->cd_view->tracks_grid, text_view_x, text_view_y));
	}
}


void cd_model_tracks_texts_color_set(CDApplication *cd_app, GdkRGBA color)
{
	cd_app->cd_model->tracks_texts_color = color;
	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_tracks_font_set(CDApplication *cd_app, char *font)
{
	int new_font_len = strlen(font);

	if (strlen(cd_app->cd_model->tracks_texts_font) < new_font_len)
	{
		cd_app->cd_model->tracks_texts_font = realloc(cd_app->cd_model->tracks_texts_font, new_font_len + 1);
	}

	strcpy(cd_app->cd_model->tracks_texts_font, font);

	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_tracks_bg_color_set(CDApplication *cd_app, GdkRGBA color)
{
	cd_app->cd_model->tracks_bg_color = color;
	_cd_model_update_css_provider(cd_app->cd_model);
}


void cd_model_tracks_spacing_set(CDApplication *cd_app, int new_spacing)
{
	cd_app->cd_model->tracks_spacing = new_spacing;
	gtk_grid_set_row_spacing(cd_app->cd_view->tracks_grid, new_spacing);
}


void cd_model_start_printing(CDApplication *cd_app)
{
	GtkPrintOperation *op = gtk_print_operation_new();
	GError *error = NULL;
	GtkPrintOperationResult res;

	GtkPrintSettings *print_settings = gtk_print_settings_new();
	GtkPageSetup     *page_setup = gtk_page_setup_new();

	gtk_print_operation_set_print_settings(op, print_settings);
	gtk_print_operation_set_default_page_setup(op, page_setup);
	gtk_print_operation_set_use_full_page(op, TRUE);
	gtk_print_operation_set_unit(op, GTK_UNIT_MM);

	g_signal_connect(op, "begin-print", G_CALLBACK(_cd_model_begin_print), cd_app);
	g_signal_connect(op, "draw-page", G_CALLBACK(_cd_model_draw_page), cd_app);

	res = gtk_print_operation_run(op, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(cd_app->cd_view->window), &error);

	if (res == GTK_PRINT_OPERATION_RESULT_ERROR)
	{
		GtkWidget *error_dialog = gtk_message_dialog_new (
			GTK_WINDOW(cd_app->cd_view->window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,
			"Error printing file:\n%s",
			error->message);
		g_signal_connect (error_dialog, "response", 
							G_CALLBACK (gtk_widget_destroy), NULL);
		gtk_widget_show (error_dialog);
		g_error_free (error);
	}
}


static void _cd_model_begin_print(GtkPrintOperation *op, GtkPrintContext *context, CDApplication *cd_app)
{
	gtk_print_operation_set_n_pages(op, 1);
}


static void _cd_model_draw_page(GtkPrintOperation *op, GtkPrintContext *context, int page_nr, CDApplication *cd_app)
{
	cairo_t *cr = gtk_print_context_get_cairo_context(context);
	double page_height = gtk_page_setup_get_page_height(gtk_print_context_get_page_setup(context), GTK_UNIT_MM);

	/* The widget containing the cover to print
	 * is always 512px by 512px, so I scale
	 * the cairo context so that 512px on
	 * screen becomes 120mm (the size of a
	 * CD cover) on the paper.
	 * 
	 * I also translate it a bit because of
	 * the printer margin.
	 */

	cairo_translate(cr, 5.0, 5.0);
	cairo_scale(cr, 120.0 / 512.0, 120.0 / 512.0);

	gtk_widget_draw(cd_app->cd_view->cover_overlay, cr);

	gtk_print_operation_draw_page_finish(op);
}


void cd_model_save_to_file(CDApplication *cd_app, char *filename)
{
	/* Here I save every element of
	 * the model structure because it
	 * contains everything we need to
	 * recreate the cover.
	 * 
	 * You can find a draft of the file
	 * structure in the doc folder,
	 * along with an example file.
	 */

	FILE *f = fopen(filename, "wb");

	/* Signature of the CD Cover file */

	char sig[3] = { 'C', 'D', 'C' };

	fwrite(&sig, sizeof(char), 3, f);

	/* Saving background image */

	int bg_img_width  = gdk_pixbuf_get_width(cd_app->cd_model->bg_image_buf);
	int bg_img_height = gdk_pixbuf_get_height(cd_app->cd_model->bg_image_buf);

	fwrite(&bg_img_width, sizeof(int), 1, f);
	fwrite(&bg_img_height, sizeof(int), 1, f);
	fwrite(gdk_pixbuf_read_pixels(cd_app->cd_model->bg_image_buf), sizeof(char), bg_img_width * bg_img_height * 4, f);

	/* Saving background tint */

	fwrite(&cd_app->cd_model->bg_tint, sizeof(double), 4, f);

	/* Saving title text */

	GtkGrid *texts_grid = cd_app->cd_view->tracks_grid;

	GtkTextView *title = GTK_TEXT_VIEW(gtk_grid_get_child_at(texts_grid, 0, 0));
	GtkTextBuffer *title_buffer = gtk_text_view_get_buffer(title);

	GtkTextIter title_start, title_end;
	gtk_text_buffer_get_bounds(title_buffer, &title_start, &title_end);

	char *title_text = gtk_text_buffer_get_text(title_buffer, &title_start, &title_end, FALSE);
	int title_text_length = strlen(title_text);

	fwrite(&title_text_length, sizeof(int), 1, f);
	fwrite(title_text, sizeof(char), title_text_length, f);

	/* Saving title style */

	fwrite(&cd_app->cd_model->title_text_color, sizeof(double), 4, f);

	int title_font_length = strlen(cd_app->cd_model->title_text_font);

	fwrite(&title_font_length, sizeof(int), 1, f);
	fwrite(cd_app->cd_model->title_text_font, sizeof(char), title_font_length, f);

	fwrite(&cd_app->cd_model->title_bg_color, sizeof(double), 4, f);

	/* Saving tracks texts */

	fwrite(&cd_app->cd_model->tracks_count, sizeof(unsigned int), 1, f);

	int track;
	for (track = 0; track < cd_app->cd_model->tracks_count; track++)
	{
		int track_x = track % 2;
		int track_y = track / 2 + 1;

		GtkTextView *track_text_view = GTK_TEXT_VIEW(gtk_grid_get_child_at(texts_grid, track_x, track_y));
		GtkTextBuffer *track_buffer = gtk_text_view_get_buffer(track_text_view);

		GtkTextIter track_buffer_start, track_buffer_end;
		gtk_text_buffer_get_bounds(track_buffer, &track_buffer_start, &track_buffer_end);

		char *track_text = gtk_text_buffer_get_text(track_buffer, &track_buffer_start, &track_buffer_end, FALSE);
		int track_text_length = strlen(track_text);

		fwrite(&track_text_length, sizeof(int), 1, f);
		fwrite(track_text, sizeof(char), track_text_length, f);
	}

	/* Saving tracks style */

	fwrite(&cd_app->cd_model->tracks_texts_color, sizeof(double), 4, f);

	int tracks_font_length = strlen(cd_app->cd_model->tracks_texts_font);

	fwrite(&tracks_font_length, sizeof(int), 1, f);
	fwrite(cd_app->cd_model->tracks_texts_font, sizeof(char), tracks_font_length, f);

	fwrite(&cd_app->cd_model->tracks_bg_color, sizeof(double), 4, f);

	fwrite(&cd_app->cd_model->tracks_spacing, sizeof(int), 1, f);

	fclose(f);
}


void cd_model_load_from_file(CDApplication *cd_app, char *filename)
{
	/* Here I load data from a file
	 * according to the file structure
	 * you can find in doc/file.
	 */

	FILE *f = fopen(filename, "rb");

	char sig[3];

	fread(&sig, sizeof(char), 3, f);

	if (memcmp(sig, "CDC", 3) != 0)
	{
		_cd_model_bad_file_dialog(cd_app);
		return;
	}

	/* Loading background image */

	/* TODO: Fix rendering bug */

	int bg_img_width, bg_img_height;

	fread(&bg_img_width, sizeof(int), 1, f);
	fread(&bg_img_height, sizeof(int), 1, f);

	unsigned char img_data[bg_img_width * bg_img_height * 4];

	int reading_pixel;
	for (reading_pixel = 0; reading_pixel < bg_img_width * bg_img_height; reading_pixel++)
	{
		fread(&img_data[reading_pixel * 4], sizeof(unsigned char), 4, f);
	}

	cd_app->cd_model->bg_image_buf = gdk_pixbuf_new_from_data(
		img_data, GDK_COLORSPACE_RGB, TRUE, 8,
		bg_img_width, bg_img_height, bg_img_width * 4, NULL, NULL
	);

	/* Loading background tint */

	fread(&cd_app->cd_model->bg_tint, sizeof(double), 4, f);
	gdk_pixbuf_fill(cd_app->cd_model->bg_tint_buf, _gdk_rgba_to_uint(&cd_app->cd_model->bg_tint));

	/* Loading title text */

	GtkGrid *tracks_grid = cd_app->cd_view->tracks_grid;

	GtkTextView *title = GTK_TEXT_VIEW(gtk_grid_get_child_at(tracks_grid, 0, 0));
	GtkTextBuffer *title_buffer = gtk_text_view_get_buffer(title);

	int title_text_length;
	
	fread(&title_text_length, sizeof(int), 1, f);

	char *title_text = calloc(title_text_length + 1, sizeof(char));

	fread(title_text, sizeof(char), title_text_length, f);

	gtk_text_buffer_set_text(title_buffer, title_text, title_text_length);

	free(title_text);

	/* Loading title style */

	fread(&cd_app->cd_model->title_text_color, sizeof(double), 4, f);

	int title_text_font_length;

	fread(&title_text_font_length, sizeof(int), 1, f);

	char *title_text_font = calloc(title_text_font_length + 1, sizeof(char));

	fread(title_text_font, sizeof(char), title_text_font_length, f);

	cd_model_title_font_set(cd_app, title_text_font);

	free(title_text_font);

	fread(&cd_app->cd_model->title_bg_color, sizeof(double), 4, f);

	/* Loading tracks texts */

	fread(&cd_app->cd_model->tracks_count, sizeof(unsigned int), 1, f);

	int track;
	for (track = 0; track < cd_app->cd_model->tracks_count; track++)
	{
		GtkWidget *new_text_view = gtk_text_view_new();

		GtkStyleContext *context = gtk_widget_get_style_context(new_text_view);
		gtk_style_context_add_class(context,"cdtrack");

		gtk_text_view_set_accepts_tab(GTK_TEXT_VIEW(new_text_view), FALSE);

		GtkTextBuffer *track_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(new_text_view));

		int track_text_length;
		fread(&track_text_length, sizeof(int), 1, f);

		char *track_text = calloc(track_text_length + 1, sizeof(char));
		fread(track_text, sizeof(char), track_text_length, f);

		gtk_text_buffer_set_text(track_buffer, track_text, track_text_length);

		free(track_text);

		int new_text_view_x = track % 2;
		int new_text_view_y = track / 2 + 1;

		gtk_grid_attach(tracks_grid, new_text_view, new_text_view_x, new_text_view_y, 1, 1);
	}

	gtk_widget_show_all(GTK_WIDGET(tracks_grid));

	/* Loading tracks style */

	fread(&cd_app->cd_model->tracks_texts_color,  sizeof(double), 4, f);

	int tracks_text_font_length;
	fread(&tracks_text_font_length, sizeof(int), 1, f);

	char *tracks_text_font = calloc(tracks_text_font_length + 1, sizeof(char));
	fread(tracks_text_font, sizeof(char), tracks_text_font_length, f);

	cd_model_tracks_font_set(cd_app, tracks_text_font);

	free(tracks_text_font);

	fread(&cd_app->cd_model->tracks_bg_color, sizeof(double), 4, f);

	fread(&cd_app->cd_model->tracks_spacing, sizeof(int), 1, f);

	fclose(f);

	cd_model_update_view(cd_app);
}


static void _cd_model_bad_file_dialog(CDApplication *cd_app)
{
	FILE *f = fopen("./glade/language", "rb");

	int language;

	fread(&language, sizeof(int), 1, f);

	fclose(f);

	GtkWidget *dialog;

	if (language == 1)
	{
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(cd_app->cd_view->window),
			GTK_DIALOG_MODAL |  GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"Erreur : Le fichier que vous avez sélectionné n'est pas une couverture de CD."
		);
	}
	else
	{
		dialog = gtk_message_dialog_new(
			GTK_WINDOW(cd_app->cd_view->window),
			GTK_DIALOG_MODAL |  GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"Error : The file you selected is not a CD cover."
		);
	}

	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}


static void _cd_model_update_css_provider(CDModel *cd_model)
{
	char css[400];
	snprintf(css, 400, CSS, 
		cd_model->title_text_font,
		_gdk_rgba_to_uint(&cd_model->title_text_color) >> 8,
		_gdk_rgba_to_uint(&cd_model->title_bg_color) >> 8,
		cd_model->tracks_texts_font,
		_gdk_rgba_to_uint(&cd_model->tracks_texts_color) >> 8,
		_gdk_rgba_to_uint(&cd_model->tracks_bg_color) >> 8);

	GdkDisplay *display = GDK_DISPLAY(gdk_display_get_default());
	GdkScreen  *screen  = GDK_SCREEN(gdk_display_get_default_screen(display));

	GError **error = NULL;
	gtk_css_provider_load_from_data(cd_model->css_provider, css, 300, error);

	gtk_style_context_add_provider_for_screen(GDK_SCREEN(screen), GTK_STYLE_PROVIDER(cd_model->css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}


static unsigned int _gdk_rgba_to_uint(GdkRGBA *gdk_rgba)
{
	return ((int)(gdk_rgba->red * 255) << 24) | ((int)(gdk_rgba->green * 255) << 16) | ((int)(gdk_rgba->blue * 255) << 8) | (int)(gdk_rgba->alpha * 255);
}
#include <gtk/gtk.h>


typedef struct CDApplication CDApplication;
typedef struct CDView CDView;
typedef struct CDModel CDModel;


struct CDApplication
{
	CDView  *cd_view;
	CDModel *cd_model;
};

extern CDApplication * application_new();
extern void            application_activate(CDApplication *cd_app);
extern int             application_run(CDApplication *cd_app, int *argc, char ***argv);
extern void            application_destroy(CDApplication *cd_app);


struct CDView
{
	GtkApplication *gtk_app;
	GtkWidget      *window;

	GtkImage       *bg_img;
	GtkImage       *bg_tint_img;

	/* Controls */
	GtkFileChooserButton *load_bg_img_btn;
	GtkColorButton       *pick_bg_tint_btn;

	GtkColorButton       *pick_title_color_btn;
	GtkFontButton        *pick_title_font_btn;
	GtkColorButton       *pick_title_bg_color_btn;

	GtkSpinButton        *tracks_count_btn;
	GtkColorButton       *pick_tracks_color_btn;
	GtkFontButton        *pick_tracks_font_btn;
	GtkColorButton       *pick_tracks_bg_color_btn;
	GtkSpinButton        *tracks_spacing_btn;

	GtkGrid        *tracks_grid;
	GtkWidget      *cover_overlay;
};

extern CDView * cd_view_new();
extern void     cd_view_application_activate(CDView *cd_view, CDApplication *cd_app);
extern int      cd_view_run(CDView *cd_view, int *argc, char ***argv);
extern void     cd_view_destroy(CDView *cd_view);

extern void     cd_view_bg_img_set(CDView *cd_view, GdkPixbuf *pixbuf);
extern void     cd_view_bg_tint_img_set(CDView *cd_view, GdkPixbuf *pixbuf);


struct CDModel
{
	GtkCssProvider *css_provider;
	
	char           *bg_image;
	GdkPixbuf      *bg_image_buf;
	GdkRGBA         bg_tint;
	GdkPixbuf      *bg_tint_buf;

	unsigned int    tracks_count;
	GdkRGBA         tracks_texts_color;
	char           *tracks_texts_font;
	GdkRGBA         tracks_bg_color;
	int             tracks_spacing;

	char           *title_text;
	GdkRGBA         title_text_color;
	char           *title_text_font;
	GdkRGBA         title_bg_color;
};

extern CDModel * cd_model_new();
extern void      cd_model_activate(CDModel *cd_model);
extern void      cd_model_destroy(CDModel *cd_model);
extern void      cd_model_update_view(CDApplication *cd_app);

extern void      cd_model_bg_image_set(CDApplication *cd_app, char *bg_image);
extern void      cd_model_bg_tint_set(CDApplication *cd_app, GdkRGBA bg_tint);

extern void      cd_model_title_bg_color_set(CDApplication *cd_app, GdkRGBA color);
extern void      cd_model_title_text_color_set(CDApplication *cd_app, GdkRGBA color);
extern void      cd_model_title_font_set(CDApplication *cd_app, char *font);

extern void      cd_model_tracks_count_set(CDApplication *cd_app, int new_tracks_count);
extern void      cd_model_tracks_texts_color_set(CDApplication *cd_app, GdkRGBA color);
extern void      cd_model_tracks_font_set(CDApplication *cd_app, char *font);
extern void      cd_model_tracks_bg_color_set(CDApplication *cd_app, GdkRGBA color);
extern void      cd_model_tracks_spacing_set(CDApplication *cd_app, int new_spacing);

extern void      cd_model_start_printing(CDApplication *cd_app);
extern void      cd_model_save_to_file(CDApplication *cd_app, char *filename);
extern void      cd_model_load_from_file(CDApplication *cd_app, char *filename);
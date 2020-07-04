#include "cdcc.h"


CDApplication * application_new()
{
	CDApplication *cd_app = malloc(sizeof(CDApplication));

	cd_app->cd_view = cd_view_new();
	cd_app->cd_model = cd_model_new();

	return cd_app;
}


void application_activate(CDApplication *cd_app)
{
	cd_view_application_activate(cd_app->cd_view, cd_app);
	cd_model_activate(cd_app->cd_model);
}


int application_run(CDApplication *cd_app, int *argc, char ***argv)
{
	int status = cd_view_run(cd_app->cd_view, argc, argv);

	return status;
}


void application_destroy(CDApplication *cd_app)
{
	cd_view_destroy(cd_app->cd_view);
	cd_model_destroy(cd_app->cd_model);
}
/* Maintainer : Alexandre Borghi <borghi.alexandre.12@gmail.com> */

#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

#include "cdcc.h"


int main(int argc, char **argv)
{
	/* We separate the application creation in 2
	 * function here because the model and the
	 * view need to be created before we can continue
	 * on with the creation of the window.
	 */
	CDApplication *app = application_new();
	application_activate(app);

	int status = application_run(app, &argc, &argv);

	application_destroy(app);

	return status;
}
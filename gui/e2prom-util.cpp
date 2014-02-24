#include <e2prom-util-gui.h>

int main (int argc, char *argv[])
{	
//	Q_INIT_RESOURCE(e2prom_util);
	QApplication app(argc, argv);//, QApplication::GuiServer);
	QApplication::setOverrideCursor(Qt::BlankCursor);
	e2prom_util_gui eu(815, 584);
	eu.showNormal();

	return app.exec();
}

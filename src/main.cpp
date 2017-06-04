#include <CQEscher.h>
#include <CQApp.h>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  CQEscherApp *escher = new CQEscherApp;

  escher->show();

  return app.exec();
}

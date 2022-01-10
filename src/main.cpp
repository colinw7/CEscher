#include <CQEscher.h>
#include <CQApp.h>

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  bool svg = false;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = QString(&argv[i][1]);

      if (arg == "svg")
        svg = true;
    }
  }

  auto *escher = new CQEscherApp;

  if (svg) {
    escher->saveSVG("escher.svg");
    exit(0);
  }
  else
    escher->show();

  return app.exec();
}

#include <CQEscher.h>
#include <CQPath2D.h>
#include <CQUtil.h>
#include <CQPenUtil.h>
#include <CQBrushUtil.h>

#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QPainter>

CQEscherApp::
CQEscherApp(QWidget *parent) :
 QFrame(parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);

  escher_ = new CQEscher;

  layout->addWidget(escher_);

  auto *control = new QFrame;

  control->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

  auto *controlLayout = new QGridLayout(control);

  borders_ = new QCheckBox;

  connect(borders_, SIGNAL(clicked(bool)), this, SLOT(updateState()));

  controlLayout->addWidget(new QLabel("Borders"), 0, 0);
  controlLayout->addWidget(borders_             , 0, 1);

  iterations_ = new QSpinBox;

  iterations_->setRange(0, 5);

  iterations_->setValue(escher_->iterations());

  connect(iterations_, SIGNAL(valueChanged(int)), this, SLOT(updateStateLater()));

  controlLayout->addWidget(new QLabel("Iterations"), 1, 0);
  controlLayout->addWidget(iterations_             , 1, 1);

  controlLayout->setRowStretch(2, 1);

  layout->addWidget(control);

  updateState();
}

void
CQEscherApp::
updateStateLater()
{
  QTimer::singleShot(100, this, SLOT(updateState()));
}

void
CQEscherApp::
updateState()
{
  escher_->setShowBorder(borders_->isChecked());
  escher_->setIterations(iterations_->value());

  escher_->addShapes();

  escher_->update();
}

void
CQEscherApp::
saveSVG(const QString &filename) const
{
  escher_->saveSVG(filename);
}

//------

CQEscher::
CQEscher(QWidget *parent) :
 QFrame(parent), CEscher()
{
  setObjectName("escher");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  addShapes();
}

void
CQEscher::
addShapes()
{
  clearShapes();

  //---

  CEscherShape blank;

  CEscherFShape f(this);

  //f.setPenColor(0, CRGBA(1,0,0));
  f.setBrushColor(0, CRGBA(0.5,0.8,0.5,0.5));

  //addShape(f);

  CEscherTriangleShape triangle(this);

  //triangle.setBrushColor(0, CRGBA(0.5,0.8,0.5,0.5));

  CEscherEdgeShape edge(this);

  //CEscherShape outline2_1 = edge.rotated();
  //CEscherShape outline2_2 = CEscherShape::rot45(edge).rotated(180);
  //CEscherShape outline2_3 = CEscherShape::rot45(edge);
  //CEscherShape outline2_4 = CEscherShape::over(outline2_2, outline2_3);

  //addShape(outline2_4);

  //CEscherShape outline2 =
  //  CEscherShape::over(CEscherShape::rot45(edge.rotated().flipped()),
  //                     CEscherShape::rot45(edge.rotated().flipped()).rotated(180));

  CEscherShape outline2 =
    CEscherShape::over(CEscherShape::rot45(edge),
                       CEscherShape::rot45(edge).rotated(180));

  //addShape(outline2);

  CEscherShape outline3 = edge.rotated().translated(-1.0, 0);

  //addShape(outline3);

  CEscherShape outline = CEscherShape::over(edge, CEscherShape::over(outline3, outline2));

  //addShape(outline);

  //addShape(f);

  //addShape(f.rotated());

  //addShape(f.rotated().flipped());
  //addShape(f.flipped().rotated());

  //addShape(f.translated(0.5, 0.5));

  //addShape(CEscherShape::beside(f, f.rotated()));
  //addShape(CEscherShape::above(f.flipped(), f));

  //addShape(CEscherShape::over(f, f.flipped()));

  //addShape(CEscherShape::quartet(f.rotated(270).flipped(), f.rotated(270),
  //                               f.rotated(), f.rotated().flipped()));

  //addShape(CEscherShape::recttri(triangle, 4));

  //addShape(CEscherShape::beside(f.flipped(), f, 2, 3));

  //addShape(CEscherShape::nonet(f, f, f, f, blank, f, f, f, f));

  CEscherFishShape fish(this);

  fish.setPenWidth(0, 0.0); fish.setBrushColor(0, CRGBA(0.5,0.8,0.5));
  fish.setPenWidth(1, 0.0); fish.setBrushColor(1, CRGBA(1.0,1.0,1.0));
  fish.setPenWidth(2, 0.0); fish.setBrushColor(2, CRGBA(1.0,1.0,1.0));
  fish.setPenWidth(3, 0.0); fish.setBrushColor(3, CRGBA(0.0,0.0,0.0));
  fish.setPenWidth(4, 0.0); fish.setBrushColor(4, CRGBA(0.0,0.0,0.0));

  fish.setRotateIndex(0);

  //addShape(fish);

  //addShape(CEscherShape::over(fish, CEscherShape::rot(CEscherShape::rot(fish))));

  CEscherShape smallfish = CEscherShape::flip(CEscherShape::rot45(fish));
  //addShape(smallfish);

  CEscherShape tfish =
    CEscherShape::over(fish, CEscherShape::over(smallfish,
      CEscherShape::rot(CEscherShape::rot(CEscherShape::rot(smallfish)))));

  //addShape(tfish);

  CEscherShape ufish =
    CEscherShape::over(
      CEscherShape::over(
       CEscherShape::over(smallfish, CEscherShape::rot(smallfish)),
       CEscherShape::rot(CEscherShape::rot(smallfish))),
      CEscherShape::rot(CEscherShape::rot(CEscherShape::rot(smallfish))));

  //addShape(ufish);

  //addShape(CEscherShape::side(tfish, 2));

  //addShape(CEscherShape::corner(tfish, ufish, 2));

  //addShape(CEscherShape::squarelimit(tfish, ufish, iterations()));

  //---

  //addShape(CEscherShape::squarelimit(f, iterations()));

  //addShape(CEscherShape::squarelimit(triangle, iterations()));

  //addShape(CEscherShape::squarelimit(outline, iterations()));

  addShape(CEscherShape::squarelimit(fish, iterations()));
}

CPath2D *
CQEscher::
createPath() const
{
  return new CQPath2D;
}

void
CQEscher::
resizeEvent(QResizeEvent *)
{
  range_.setPixelRange(0, 0, width() - 1, height() - 1);

  range_.setWindowRange(0, 0, 1, 1);

  range_.setEqualScale(true);
}

void
CQEscher::
paintEvent(QPaintEvent *)
{
  if (! painter_)
    painter_ = new QPainter;

  painter_->begin(this);

  painter_->setRenderHints(QPainter::Antialiasing);

  painter_->fillRect(rect(), Qt::white);

  painter_->setWorldMatrix(CQUtil::toQMatrix(range_.getMatrix()));

  for (const auto &shape : shapes()) {
    shape.draw(this);
  }

  painter_->end();
}

void
CQEscher::
setPen(const CPen &pen)
{
  if (svgFp_) {
    svgFg_ = pen.getColor();
  }
  else {
    painter_->setPen(CQPenUtil::toQPen(pen));
  }
}

void
CQEscher::
setBrush(const CBrush &brush)
{
  if (svgFp_) {
    svgBg_ = brush.getColor();
  }
  else {
    painter_->setBrush(CQBrushUtil::toQBrush(brush));
  }
}

void
CQEscher::
drawLine(const CPoint2D &p1, const CPoint2D &p2)
{
  if (svgFp_) {
    auto p1t = transformPoint(p1);
    auto p2t = transformPoint(p2);

    auto cmd = QString("<path d=\"M%1,%2 L%3,%4\" style=\"stroke: #000000; fill: none;\"/>\n").
                arg(p1t.x).arg(p1t.y).arg(p2t.x).arg(p2t.y);

    fputs(cmd.toStdString().c_str(), svgFp_);
  }
  else {
    painter_->drawLine(CQUtil::toQPoint(p1), CQUtil::toQPoint(p2));
  }
}

void
CQEscher::
stroke(CPath2D *path)
{
  auto *qpath = dynamic_cast<CQPath2D *>(path);
  if (! qpath) return;

  if (svgFp_) {
    auto str = pathToSVGString(qpath);

    auto colorStr = svgFg_.getRGB().stringEncode();

    auto cmd = QString("<path d=\"%1\" style=\"stroke: %2; fill: none;\"/>\n").
                 arg(str).arg(QString::fromStdString(colorStr));

    fputs(cmd.toStdString().c_str(), svgFp_);
  }
  else {
    qpath->stroke(painter_);
  }
}

void
CQEscher::
fill(CPath2D *path)
{
  auto *qpath = dynamic_cast<CQPath2D *>(path);
  if (! qpath) return;

  if (svgFp_) {
    auto str = pathToSVGString(qpath);

    auto colorStr = svgBg_.getRGB().stringEncode();

    auto cmd = QString("<path d=\"%1\" style=\"stroke: none; fill: %2;\"/>\n").
                 arg(str).arg(QString::fromStdString(colorStr));

    fputs(cmd.toStdString().c_str(), svgFp_);
  }
  else {
    qpath->fill(painter_);
  }
}

void
CQEscher::
saveSVG(const QString &filename) const
{
  auto *th = const_cast<CQEscher *>(this);

  svgFp_ = fopen(filename.toStdString().c_str(), "w");
  if (! svgFp_) return;

  auto cmd = QString("<svg viewBox=\"0 0 %1 %2\">\n").arg(svgSize_).arg(svgSize_);

  fputs(cmd.toStdString().c_str(), svgFp_);

  for (const auto &shape : shapes()) {
    shape.draw(th);
  }

  fputs("</svg>\n", svgFp_);

  fclose(svgFp_);

  svgFp_ = nullptr;
}

QString
CQEscher::
pathToSVGString(CQPath2D *path) const
{
  class PathVisitor : public CQPath2D::PathVisitor {
   public:
    PathVisitor(CQEscher *escher) :
     escher_(escher) {
    }

    void moveTo(const CPoint2D &p) override {
      auto pt = escher_->transformPoint(p);

      if (str_.length()) str_ += " ";

      str_ += QString("M %1 %2").arg(pt.x).arg(pt.y);
    }

    void lineTo(const CPoint2D &p) override {
      auto pt = escher_->transformPoint(p);

      if (str_.length()) str_ += " ";

      str_ += QString("L %1 %2").arg(pt.x).arg(pt.y);
    }

    void quadTo(const CPoint2D &p1, const CPoint2D &p2) override {
      auto p1t = escher_->transformPoint(p1);
      auto p2t = escher_->transformPoint(p2);

      if (str_.length()) str_ += " ";

      str_ += QString("Q %1 %2 %3 %4").arg(p1t.x).arg(p1t.y).arg(p2t.x).arg(p2t.y);
    }

    void curveTo(const CPoint2D &p1, const CPoint2D &p2, const CPoint2D &p3) override {
      auto p1t = escher_->transformPoint(p1);
      auto p2t = escher_->transformPoint(p2);
      auto p3t = escher_->transformPoint(p3);

      if (str_.length()) str_ += " ";

      str_ += QString("C %1 %2 %3 %4 %5 %6").
                arg(p1t.x).arg(p1t.y).arg(p2t.x).arg(p2t.y).arg(p3t.x).arg(p3t.y);
    }

    const QString &str() const { return str_; }

   private:
    CQEscher *escher_ { nullptr };
    QString   str_;
  };

  auto *th = const_cast<CQEscher *>(this);

  PathVisitor visitor(th);

  path->visit(visitor);

  return visitor.str();
}

CPoint2D
CQEscher::
transformPoint(const CPoint2D &p) const
{
  return CPoint2D(p.x*svgSize_, p.y*svgSize_);
}

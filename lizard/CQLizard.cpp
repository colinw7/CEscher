#include <CQLizard.h>
#include <CQPropertyViewTree.h>

#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QHBoxLayout>

#include <iostream>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  app.setFont(QFont("Sans", 16));

  auto *lapp = new CQLizardApp;

  lapp->show();

  return app.exec();
}

//---

CQLizardApp::
CQLizardApp(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = new QHBoxLayout(this);

  lizard_ = new CQLizard;

  layout->addWidget(lizard_);

  viewTree_ = new CQPropertyViewTree(this);

  layout->addWidget(viewTree_);

  viewTree_->addProperty("", lizard_, "center" );
  viewTree_->addProperty("", lizard_, "radius" );
  viewTree_->addProperty("", lizard_, "rcenter");
  viewTree_->addProperty("", lizard_, "rangle" );

  viewTree_->setFixedWidth(350);
}

//---

CQLizard::
CQLizard(QWidget *parent) :
 QFrame(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  radius_ = 32.125;

  auto dcos = [&](double a) { return std::cos(M_PI*a/180.0); };
  auto dsin = [&](double a) { return std::sin(M_PI*a/180.0); };

  dx_[0] =  0.0;
  dy_[0] =  0.0;
  dx_[1] =  3.0*radius_*dcos(30);
  dy_[1] =  3.0*radius_*dsin(30);
  dx_[2] =  0.0;
  dy_[2] =  3.0*radius_;
  dx_[3] = -3.0*radius_*dcos(30);
  dy_[3] =  3.0*radius_*dsin(30);
  dx_[4] = -3.0*radius_*dcos(30);
  dy_[4] = -3.0*radius_*dsin(30);
  dx_[5] =  0.0;
  dy_[5] = -3.0*radius_;
  dx_[6] =  3.0*radius_*dcos(30);
  dy_[6] = -3.0*radius_*dsin(30);

  lizardPoints();
}

void
CQLizard::
lizardPoints()
{
  Points points = {
    QPointF(107.977, 103.753),
    QPointF(10.9747, -9.375),
    QPointF(-1.80432, -2.80878),
    QPointF(-4.14807, -1.02307),
    QPointF(-5.20833, -2.28795),
    QPointF(-1.71131, 3.44122),
    QPointF(2.08333, 4.16667),
    QPointF(-7.34747, 4.6317),
    QPointF(0.0186012, 4.07366),
    QPointF(0.533885, 2.94824),
    QPointF(0.619389, 4.64104),
    QPointF(0.260417, 3.1436),
    QPointF(-10.4991, 13.4077),
    QPointF(2.75354, -11.6473),
    QPointF(-2.07719, -11.1232),
    QPointF(-4.09841, -3.63426),
    QPointF(-10.5074, 0.576637),
    QPointF(-4.18169, -0.92534),
    QPointF(-6.38778, 5.11022),
    QPointF(6.64462, 7.68268),
    QPointF(9.96801, -1.86012),
    QPointF(1.22768, 8.07961),
    QPointF(-2.88538, 3.15694),
    QPointF(-4.80962, 9.39379),
    QPointF(-5.13759, 1.70969),
    QPointF(-10.644, -3.73875),
    QPointF(-8.71743, -8.19138),
    QPointF(6.31263, 15.7816),
    QPointF(4.82855, 2.15671),
    QPointF(5.26414, 1.99033),
    QPointF(3.47842, 2.00893),
    QPointF(1.53411, 0.0815107),
    QPointF(10.435, 1.075),
    QPointF(4.21044, 6.42363),
    QPointF(3.61603, 4.82602),
    QPointF(-8.41683, 5.93687),
    QPointF(-3.38176, 4.58417),
    QPointF(7.74048, 6.83868),
    QPointF(2.63026, -6.08717),
    QPointF(10.6055, -6.0738),
    QPointF(0.0372024, -11.9792),
    QPointF(-3.92485, -5.35714),
    QPointF(9.76562, -3.25521),
    QPointF(2.30655, 5.24554),
    QPointF(0.93006, 10.1935),
    QPointF(6.7328, -0.722552),
    QPointF(6.91601, -0.430722),
    QPointF(6.99405, -9.76562),
    QPointF(-7.67257, -4.08217),
    QPointF(-2.54451, 3.92118),
    QPointF(-3.83259, 0.21591),
    QPointF(-0.65828, -5.57382),
    QPointF(-1.22458, -4.18452),
    QPointF(1.52747, -2.95365),
    QPointF(13.6131, 4.7936),
    QPointF(9.99221, -7.80234),
    QPointF(0.61004, -4.34137),
    QPointF(-11.8073, -12.0016),
    QPointF(-16.3094, 4.23544),
    QPointF(-3.4757, -2.7242),
  };

  points_.clear();

  QPointF p(0, 0);

  for (const auto &p1 : points) {
    double x1 = p.x() + p1.x();
    double y1 = p.y() + p1.y();

    p = QPointF(x1, y1);

    points_.push_back(p);
  }

  for (auto &p : points_)
    p.setY(240 - p.y());

//rcenter_ = QPointF(129.513, 125.236);
  rcenter_ = points_[57];

  assert(pointInd(rcenter_) >= 0);

  rangle_ = 30.0;

  auto dcos = [&](double a) { return std::cos(M_PI*a/180.0); };
  auto dsin = [&](double a) { return std::sin(M_PI*a/180.0); };

//center_ = QPointF(101.69, 109.18);
  center_ = QPointF(rcenter_.x() - radius_*dcos(rangle_), rcenter_.y() - radius_*dsin(rangle_));

#if 0
  double l = std::hypot(rcenter_.x() - center_.x(), rcenter_.y() - center_.y());
  double a = 180.0*std::atan2(rcenter_.y() - center_.y(), rcenter_.x() - center_.x())/M_PI;

  std::cerr << "center dist = " << l << ", angle = " << a << "\n";
#endif
}

void
CQLizard::
resetPoints()
{
  auto rotatePoint = [&](const QPointF &p, const QPointF &center, double da) {
    double da1 = M_PI*da/180.0;

    double s = std::sin(da1);
    double c = std::cos(da1);

    double x1 = p.x() - center.x();
    double y1 = p.y() - center.y();

    double x2 = x1*c - y1*s;
    double y2 = x1*s + y1*c;

    return QPointF(x2 + center.x(), y2 + center.y());
  };

  auto norm = [&](double x, double low, double high) {
    if (high != low)
      return (x - low)/(high - low);
    else
      return low;
  };

  auto lerp = [&](double x, double low, double high) {
    return low + (high - low)*x;
  };

  auto map = [&](double value, double low1, double high1, double low2, double high2) {
    return lerp(norm(value, low1, high1), low2, high2);
  };

  //---

  points_.clear();

  double a = 0.0;

  QPointF p(center_.x(), center_.y() + radius_);

  QPointF p1 = p;

  for (int i = 0; i < 6; ++i) {
    points_.push_back(p1);

    a += 60.0;

    auto p2 = rotatePoint(p, center_, a);

    int nj = 7;

    for (int j = 0; j < nj; ++j) {
      double f = (j + 1)*1.0/(nj + 1.0);

      double x = map(f, 0, 1, p1.x(), p2.x());
      double y = map(f, 0, 1, p1.y(), p2.y());

      points_.push_back(QPointF(x, y));
    }

    p1 = p2;
  }

  //rcenter_ = QPointF(129.513, 125.236);
  rcenter_ = points_[40];

  assert(pointInd(rcenter_) >= 0);

  auto dcos = [&](double a) { return std::cos(M_PI*a/180.0); };
  auto dsin = [&](double a) { return std::sin(M_PI*a/180.0); };

//center_ = QPointF(101.69, 109.18);
  center_ = QPointF(rcenter_.x() - radius_*dcos(rangle_), rcenter_.y() - radius_*dsin(rangle_));
}

void
CQLizard::
resizeEvent(QResizeEvent *)
{
  range_.setPixelRange(0, 0, width() - 1, height() - 1);

  range_.setWindowRange(55, 55, 205, 205);

  range_.setEqualScale(true);
}

void
CQLizard::
paintEvent(QPaintEvent *)
{
  auto rotatePoint = [&](const QPointF &p, const QPointF &center, double da) {
    double da1 = M_PI*da/180.0;

    double s = std::sin(da1);
    double c = std::cos(da1);

    double x1 = p.x() - center.x();
    double y1 = p.y() - center.y();

    double x2 = x1*c - y1*s;
    double y2 = x1*s + y1*c;

    return QPointF(x2 + center.x(), y2 + center.y());
  };

  //---

  double angles[] = { 0, 120, 240 };
  QColor colors[] = { QColor(200, 100, 100), QColor(100, 200, 100), QColor(100, 100, 200) };

  QPainter painter(this);

  int is = 0;

  for (auto &a : angles) {
    auto c = colors[is];

    for (int i = 0; i < 7; ++i) {
      auto dx = dx_[i];
      auto dy = dy_[i];

      QPainterPath path;

      for (const auto &p : points_) {
        auto p1 = rotatePoint(p, rcenter_, a);

        double x, y;

        range_.windowToPixel(p1.x() + dx, p1.y() + dy, &x, &y);

        if (path.elementCount() == 0)
          path.moveTo(x, y);
        else
          path.lineTo(x, y);
      }

      path.closeSubpath();

      if (i == 0)
        c.setAlphaF(0.6);
      else
        c.setAlphaF(0.3);

      painter.setPen(Qt::black);
      painter.setBrush(c);

      painter.drawPath(path);
    }

    ++is;
  }

  //---

  {
  auto p = points_[mouseInd_];

  int r = 6;

  double x, y;

  range_.windowToPixel(p.x(), p.y(), &x, &y);

  painter.setPen(Qt::black);
  painter.setBrush(Qt::red);

  painter.drawEllipse(QRectF(x - r, y - r, 2*r, 2*r));
  }

  //---

  painter.setPen(Qt::black);

  QFontMetricsF fm(font());

  auto text = QString("%1,%2").arg(mousePos_.x()).arg(mousePos_.y());

  painter.drawText(4, height() - fm.descent() - 4, text);

  //---

  for (auto &a : angles) {
    auto center1 = rotatePoint(center_, rcenter_, a);

    QPainterPath path;

    double  a1 = 0.0;
    QPointF p  = QPointF(center1.x(), center1.y() + radius_);

    for (int i = 0; i < 6; ++i) {
      auto p1 = rotatePoint(p, center1, a1);

      double x, y;

      range_.windowToPixel(p1.x(), p1.y(), &x, &y);

      if (i == 0)
        path.moveTo(QPointF(x, y));
      else
        path.lineTo(QPointF(x, y));

      a1 += 60.0;
    }

    path.closeSubpath();

    painter.setPen(Qt::black);

    painter.strokePath(path, painter.pen());
  }
}

void
CQLizard::
mousePressEvent(QMouseEvent *me)
{
  auto pos = me->pos();

  updateMousePos(pos);

  pressed_ = true;
  button_  = me->button();

  update();
}

void
CQLizard::
mouseMoveEvent(QMouseEvent *me)
{
  auto pos = me->pos();

  updateMousePos(pos);

  if (pressed_) {
    double x, y;

    range_.pixelToWindow(pos.x(), pos.y(), &x, &y);

    QPointF p(x, y);

    if      (button_ == Qt::LeftButton) {
      points_[mouseInd_] = p;
    }
    else if (button_ == Qt::MidButton) {
      rcenter_ = p;
    }
    else if (button_ == Qt::RightButton) {
      center_ = p;
    }
  }

  update();
}

void
CQLizard::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
}

void
CQLizard::
updateMousePos(const QPoint &pos)
{
  double x, y;

  range_.pixelToWindow(pos.x(), pos.y(), &x, &y);

  mousePos_ = QPointF(x, y);

  double d   = 9999;
  int    ind = 0;

  for (const auto &p : points_) {
    double d1 = std::hypot(p.x() - x, p.y() - y);

    if (d1 < d) {
      mouseInd_ = ind;
      d         = d1;
    }

    ++ind;
  }
}

int
CQLizard::
pointInd(const QPointF &p) const
{
  int    ind  = 0;
  double d    = 9999;
  int    dind = 0;

  for (const auto &p1 : points_) {
    double d1 = std::hypot(p1.x() - p.x(), p1.y() - p.y());

    if (p1 == p)
      return ind;

    if (d1 < d) {
      dind = ind;
      d    = d1;
    }

    ++ind;
  }

  assert(false);

  return -1;
}

void
CQLizard::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_P) {
    int i = 0;

    QPointF p1;

    for (auto &p : points_) {
      auto pp = p;

      pp.setY(240 - p.y());

      if (i == 0)
        std::cerr << "  QPointF(" << pp.x() << ", " << pp.y() << "),\n";
      else {
        double dx = pp.x() - p1.x();
        double dy = pp.y() - p1.y();

        std::cerr << "  QPointF(" << dx << ", " << dy << "),\n";
      }

      p1 = pp;

      ++i;
    }
  }
  else if (e->key() == Qt::Key_S) {
    int i = 0;

    QPointF p1;

    for (auto &p : points_) {
      auto pp = p;

      pp.setY(240 - p.y());

      if (i == 0)
        std::cerr << "m " << pp.x() << ", " << pp.y();
      else {
        double dx = pp.x() - p1.x();
        double dy = pp.y() - p1.y();

        std::cerr << " " << dx << ", " << dy;
      }

      p1 = pp;

      ++i;
    }

    std::cerr << " z\n";
  }
  else if (e->key() == Qt::Key_R) {
    resetPoints();

    update();
  }
  else if (e->key() == Qt::Key_Plus) {
    range_.zoomIn();

    update();
  }
  else if (e->key() == Qt::Key_Minus) {
    range_.zoomOut();

    update();
  }
  else if (e->key() == Qt::Key_Left) {
    range_.scrollX(-1.0);

    update();
  }
  else if (e->key() == Qt::Key_Right) {
    range_.scrollX(1.0);

    update();
  }
  else if (e->key() == Qt::Key_Up) {
    range_.scrollY(1.0);

    update();
  }
  else if (e->key() == Qt::Key_Down) {
    range_.scrollY(-1.0);

    update();
  }
}

QSize
CQLizard::
sizeHint() const
{
  return QSize(1000, 1000);
}

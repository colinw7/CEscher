#include <CQTessel.h>
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

  auto *lapp = new CQTesselApp;

  lapp->show();

  return app.exec();
}

CQTesselApp::
CQTesselApp(QWidget *parent) :
 QFrame(parent)
{
  auto *layout = new QHBoxLayout(this);

  tessel_ = new CQTessel;

  layout->addWidget(tessel_);

  viewTree_ = new CQPropertyViewTree(this);

  layout->addWidget(viewTree_);

  viewTree_->addProperty("", tessel_, "style"   );
  viewTree_->addProperty("", tessel_, "pattern" );
  viewTree_->addProperty("", tessel_, "center"  );
  viewTree_->addProperty("", tessel_, "radius"  );
  viewTree_->addProperty("", tessel_, "angle"   );
  viewTree_->addProperty("", tessel_, "xscale"  );
  viewTree_->addProperty("", tessel_, "yscale"  );
  viewTree_->addProperty("", tessel_, "numEdge" );
  viewTree_->addProperty("", tessel_, "gridSize");

  viewTree_->setFixedWidth(350);
}

CQTessel::
CQTessel(QWidget *parent) :
 QFrame(parent)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  setMouseTracking(true);

  setFocusPolicy(Qt::StrongFocus);

  radius_ = 1.0;

  resetPoints();
}

void
CQTessel::
resetPoints()
{
  edges_.clear();

  ne_ = 0;
  np_ = 0;

  int nj = numEdge_ - 1;

  double a = 0.0;

  np_ = numEdge_;

  if      (style() == Style::TRIANGLE) {
    ne_ = 3;

    QPointF p(center_.x(), center_.y() + radius_);

    QPointF p1 = p;

    for (int i = 0; i < ne_; ++i) {
      Edge edge;

      edge.transform = EdgeTransform::SYMMETRIC_MIRROR;

      edge.angle = a + 240.0;
      edge.p1    = p1;

      edge.points.push_back(QPointF(0.0, 0.0));

      a += 120.0;

      auto p2 = rotatePoint(p, center_, a);

      edge.p2 = p2;

      for (int j = 0; j < nj; ++j) {
        double f = 1.0*(j + 1)/nj;

        edge.points.push_back(QPointF(f, 0.0));
      }

      edges_.push_back(edge);

      p1 = p2;
    }
  }
  else if (style() == Style::SQUARE) {
    ne_ = 4;

    QPointF p(center_.x() + radius_, center_.y() + radius_);

    QPointF p1 = p;

    for (int i = 0; i < ne_; ++i) {
      Edge edge;

      if      (pattern() == Pattern::OPPOSITE) {
        // mirror first horiz and first vert side
        if (i == 2 || i == 3) {
          edge.type      = EdgeType::REFERENCE;
          edge.ind       = (i == 2 ? 0 : 1);
          edge.transform = EdgeTransform::MIRROR_BOTH;
        }
        else {
          edge.type      = EdgeType::POINTS;
          edge.transform = EdgeTransform::NONE;
        }
      }
      else if (pattern() == Pattern::ADJACENT) {
        // mirror first horiz and first vert side
        if (i == 2 || i == 3) {
          edge.type      = EdgeType::REFERENCE;
          edge.ind       = (i == 2 ? 1 : 0);
          edge.transform = EdgeTransform::MIRROR_PARALLEL;
        }
        else {
          edge.type      = EdgeType::POINTS;
          edge.transform = EdgeTransform::NONE;
        }
      }
      else if (pattern() == Pattern::REPEAT_ALL) {
        if (i != 0) {
           edge.type = EdgeType::REFERENCE;
           edge.ind  = 0;
        }

        if      (i == 1)
          edge.transform = EdgeTransform::MIRROR_PERP;
        else if (i == 2)
          edge.transform = EdgeTransform::MIRROR_BOTH;
        else if (i == 3)
          edge.transform = EdgeTransform::MIRROR_PARALLEL;
      }
      else if (pattern() == Pattern::TWO_TILE) {
        if      (i == 0 || i == 2)
          edge.transform = EdgeTransform::SYMMETRIC;
        else if (i == 3) {
          edge.type      = EdgeType::REFERENCE;
          edge.ind       = 1;
          edge.transform = EdgeTransform::MIRROR_PARALLEL;
        }
      }

      edge.angle = a + 180.0;
      edge.p1    = p1;

      edge.points.push_back(QPointF(0.0, 0.0));

      a += 90.0;

      auto p2 = rotatePoint(p, center_, a);

      edge.p2 = p2;

      for (int j = 0; j < nj; ++j) {
        double f = 1.0*(j + 1)/nj;

        edge.points.push_back(QPointF(f, 0.0));
      }

      edges_.push_back(edge);

      p1 = p2;
    }
  }
  else if (style() == Style::HEXAGON) {
    ne_ = 6;

    QPointF p(center_.x(), center_.y() + radius_);

    QPointF p1 = p;

    for (int i = 0; i < ne_; ++i) {
      Edge edge;

      if (i == 1 || i == 3 || i == 5) {
        edge.type      = EdgeType::REFERENCE;
        edge.ind       = i - 1;
        edge.transform = EdgeTransform::MIRROR_BOTH;
      }

      edge.angle = a + 210.0;
      edge.p1    = p1;

      edge.points.push_back(QPointF(0.0, 0.0));

      a += 60.0;

      auto p2 = rotatePoint(p, center_, a);

      edge.p2 = p2;

      for (int j = 0; j < nj; ++j) {
        double f = 1.0*(j + 1)/nj;

        edge.points.push_back(QPointF(f, 0.0));
      }

      edges_.push_back(edge);

      p1 = p2;
    }
  }

  assert(int(edges_.size()) == ne_);
  assert(int(edges_[0].points.size()) == np_);

  npe_ = ne_*np_;

  mouseData_.reset();
}

CQTessel::TileBox
CQTessel::
tileBox(int row, int col) const
{
  // 0, 0 is center
  if (row == 0 && col == 0)
    return TileBox();

  if      (style() == Style::TRIANGLE) {
    double ra = deg2Rad(30.0);

    double s = std::sin(ra);
    double c = std::cos(ra);

    double a = 0.0;

    int ic = 0;

    bool flip = ((std::abs(row) + std::abs(col)) & 1);

    double dx = c*radius_*col;

    double dy = (s + 1)*radius_*row;

    if (flip) {
      a  = 180.0;
      ic = 1;

      dy -= s*radius_;
    }

    return TileBox(dx, dy, a, ic);
  }
  else if (style() == Style::SQUARE) {
    double dx = 2*radius_*col;
    double dy = 2*radius_*row;

    TileBox tileBox(dx, dy, 0.0, 1);

    if (pattern() == Pattern::TWO_TILE || pattern() == Pattern::ADJACENT) {
      if ((std::abs(row) + std::abs(col)) & 1)
        tileBox.visible = false;
    }

    return tileBox;
  }
  else if (style() == Style::HEXAGON) {
    double ra = deg2Rad(30.0);

    double s = std::sin(ra);
    double c = std::cos(ra);

    double dx = 2*radius_*c*col;
    double dy = 3*radius_*s*row;

    if (std::abs(row) & 1) // odd
      dx -= radius_*c;

    int dc = 0;

    int ir = (std::abs(row) % 5);

    if      (ir == 1) {
      if (row > 0)
        dc = 0;
      else
        dc = 2;
    }
    else if (ir == 2) {
      if (row > 0)
        dc = 1;
      else
        dc = 2;
    }
    else if (ir == 3) {
      if (row > 0)
        dc = 1;
      else
        dc = 1;
    }
    else if (ir == 4) {
      if (row > 0)
        dc = 0;
      else
        dc = 1;
    }

    int irow, icol;

    if (row < 0)
      irow = 2 - (-(row + 1) % 3);
    else
      irow = row % 3;

    if (col < 0)
      icol = 2 - (-(col + 1) % 3);
    else
      icol = col % 3;

    int ic = ((icol + irow + dc) % 3);

    return TileBox(dx, dy, ic*120.0, ic);
  }
  else
    return TileBox();
}

void
CQTessel::
resizeEvent(QResizeEvent *)
{
  range_.setPixelRange(0, 0, width() - 1, height() - 1);

  range_.setWindowRange(-4, -4, 4, 4);

  range_.setEqualScale(true);
}

void
CQTessel::
paintEvent(QPaintEvent *)
{
  QColor colors[] = {
    QColor(200, 100, 100),
    QColor(100, 200, 100),
    QColor(100, 100, 200)
  };

  QPainter painter(this);

  int ng = gridSize();

  for (int ir = -ng; ir <= ng; ++ir) {
    for (int ic = -ng; ic <= ng; ++ic) {
      auto tileBox = this->tileBox(ir, ic);

      if (! tileBox.visible)
        continue;

      auto c = colors[tileBox.colorInd];

      //auto center_dx = movePointOnLine  (center_  , angle_, xscale_*tileBox.dx);
      //auto center_dy = movePointPerpLine(center_dx, angle_, yscale_*tileBox.dy);

      QPainterPath path;

      for (int ip = 0; ip < npe_; ++ip) {
        QPointF p;

        if (ir != 0 || ic != 0) {
          auto p1 = getPoint(ip, /*transform*/false);

          auto pr1 = rotatePoint(p1 , center_, tileBox.angle);
          auto pr2 = rotatePoint(pr1, center_, angle_);

          auto pdx = movePointOnLine  (pr2, angle_, tileBox.dx);
          auto pdy = movePointPerpLine(pdx, angle_, tileBox.dy);

          auto ps = scalePoint(xscale_, yscale_, pdy, center_);

          p = ps;
        }
        else {
          p = getPoint(ip);
        }

        auto pp = windowToPixel(p);

        if (path.elementCount() == 0)
          path.moveTo(pp.x(), pp.y());
        else
          path.lineTo(pp.x(), pp.y());
      }

      path.closeSubpath();

      if (ir == 0 && ic == 0)
        c.setAlphaF(0.6);
      else
        c.setAlphaF(0.3);

      painter.setPen(Qt::black);
      painter.setBrush(c);

      painter.drawPath(path);
    }
  }

  //---

  // draw current point
  {
  auto p = getPoint(mouseData_.ind);

  double r = 6.0; // pixels

  auto pp = windowToPixel(p);

  painter.setPen(Qt::black);
  painter.setBrush(Qt::red);

  painter.drawEllipse(QRectF(pp.x() - r, pp.y() - r, 2*r, 2*r));

  pp = windowToPixel(mouseData_.edgePos);

  painter.setBrush(Qt::green);

  painter.drawEllipse(QRectF(pp.x() - r, pp.y() - r, 2*r, 2*r));
  }

  //---

  // draw mouse pos
  int ie = mouseData_.ind / np_;
  int ip = mouseData_.ind % np_;

  painter.setPen(Qt::black);

  QFontMetricsF fm(font());

  auto text = QString("%1,%2 [%3,%4]").
                arg(mouseData_.pos.x()).arg(mouseData_.pos.y()).arg(ie).arg(ip);

  painter.drawText(4, height() - fm.descent() - 4, text);

  //---

  // draw grid
  {
  QPainterPath path;

  if (style() == Style::SQUARE) {
    QPointF p1( radius_,  radius_);
    QPointF p2(-radius_,  radius_);
    QPointF p3(-radius_, -radius_);
    QPointF p4( radius_, -radius_);

    auto pt1 = scalePoint(xscale_, yscale_, rotatePoint(p1, center_, angle_), center_);
    auto pt2 = scalePoint(xscale_, yscale_, rotatePoint(p2, center_, angle_), center_);
    auto pt3 = scalePoint(xscale_, yscale_, rotatePoint(p3, center_, angle_), center_);
    auto pt4 = scalePoint(xscale_, yscale_, rotatePoint(p4, center_, angle_), center_);

    path.moveTo(windowToPixel(pt1));
    path.lineTo(windowToPixel(pt2));
    path.lineTo(windowToPixel(pt3));
    path.lineTo(windowToPixel(pt4));
  }

  path.closeSubpath();

  QPen pen(QColor(100, 100, 100));

  pen.setStyle(Qt::DashLine);

  painter.strokePath(path, pen);
  }
}

QPointF
CQTessel::
windowToPixel(const QPointF &w) const
{
  double px, py;

  range_.windowToPixel(w.x(), w.y(), &px, &py);

  return QPointF(px, py);
}

QPointF
CQTessel::
pixelToWindow(const QPointF &p) const
{
  double wx, wy;

  range_.pixelToWindow(p.x(), p.y(), &wx, &wy);

  return QPointF(wx, wy);
}

void
CQTessel::
mousePressEvent(QMouseEvent *me)
{
  auto pos = me->pos();

  updateMousePos(pos);
  updateNearest (pos);

  mouseData_.pressed = true;
  mouseData_.button  = me->button();

  mouseData_.pressPoint = getPoint(mouseData_.ind);

  update();
}

void
CQTessel::
mouseMoveEvent(QMouseEvent *me)
{
  auto pos = me->pos();

  if (mouseData_.pressed) {
    auto p = pixelToWindow(pos);

    if  (mouseData_.button == Qt::LeftButton) {
      setPoint(mouseData_.ind, p);
    }
  }
  else {
    updateMousePos(pos);
  }

  updateNearest(pos);

  update();
}

void
CQTessel::
mouseReleaseEvent(QMouseEvent *)
{
  mouseData_.pressed = false;
}

void
CQTessel::
updateMousePos(const QPoint &pos)
{
  mouseData_.pos = pixelToWindow(pos);

  double d = 9999;

  for (int i = 0; i < npe_; ++i) {
    auto p = getPoint(i);

    double d1 = std::hypot(p.x() - mouseData_.pos.x(), p.y() - mouseData_.pos.y());

    if (d1 < d) {
      mouseData_.ind = i;
      d              = d1;
    }
  }
}

void
CQTessel::
updateNearest(const QPoint &pos)
{
  auto wpos = pixelToWindow(pos);

  int ie = mouseData_.ind / np_;
  int ip = mouseData_.ind % np_;

  assert(ie >= 0 && ie < ne_ && ip >= 0 && ip <= np_);

  //---

  auto point1 = rotatePoint(wpos, center_, -angle_);

  //---

  auto &edge = edges_[ie];

  auto p1 = scalePoint(xscale_, yscale_, edge.p1, center_);
  auto p2 = scalePoint(xscale_, yscale_, edge.p2, center_);

  double s, t;

  pointToEdge(point1, p1, p2, s, t);

  //---

  double l = std::hypot(p2.x() - p1.x(), p2.y() - p1.y());

  mouseData_.edgePos = movePointPerpLine(mapPointOnLine(p1, p2, s), edge.angle, t*l);

  mouseData_.edgePos = rotatePoint(mouseData_.edgePos, center_, angle_);
}

void
CQTessel::
keyPressEvent(QKeyEvent *e)
{
  if      (e->key() == Qt::Key_P) {
    QPointF p1;

    for (int i = 0; i < npe_; ++i) {
      auto p = getPoint(i);

      if (i == 0)
        std::cerr << "  QPointF(" << p.x() << ", " << p.y() << "),\n";
      else {
        double dx = p.x() - p1.x();
        double dy = p.y() - p1.y();

        std::cerr << "  QPointF(" << dx << ", " << dy << "),\n";
      }

      p1 = p;
    }
  }
  else if (e->key() == Qt::Key_S) {
    QPointF p1;

    for (int i = 0; i < npe_; ++i) {
      auto p = getPoint(i);

      if (i == 0)
        std::cerr << "m " << p.x() << ", " << p.y();
      else {
        double dx = p.x() - p1.x();
        double dy = p.y() - p1.y();

        std::cerr << " " << dx << ", " << dy;
      }

      p1 = p;
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

QPointF
CQTessel::
getPoint(int i, bool transform) const
{
  int ie = i / np_;
  int ip = i % np_;

  assert(ie >= 0 && ie < ne_ && ip >= 0 && ip <= np_);

  //---

  const auto &edge = edges_[ie];

  //auto p1 = scalePoint(xscale_, yscale_, edge.p1, center_);
  //auto p2 = scalePoint(xscale_, yscale_, edge.p2, center_);
  auto p1 = edge.p1;
  auto p2 = edge.p2;

  //---

  bool mirrorParallel = false;
  bool mirrorPerp     = false;

  QPointF point;

  if (edge.type == EdgeType::REFERENCE) {
    mirrorParallel = (edge.transform == EdgeTransform::MIRROR_BOTH ||
                      edge.transform == EdgeTransform::MIRROR_PARALLEL);
    mirrorPerp     = (edge.transform == EdgeTransform::MIRROR_BOTH ||
                      edge.transform == EdgeTransform::MIRROR_PERP);

    const auto &refEdge = edges_[edge.ind];

    double s, t;

    QPointF p;

    if (mirrorParallel || mirrorPerp) {
      if (mirrorParallel) {
        int ip1 = np_ - 1 - ip;

        s = refEdge.points[ip1].x();
        t = refEdge.points[ip1].y();
      }
      else {
        s = refEdge.points[ip].x();
        t = refEdge.points[ip].y();
      }

      if (mirrorPerp)
        t = -t;

      if (mirrorParallel)
        p = mapPointOnLine(p2, p1, s);
      else
        p = mapPointOnLine(p1, p2, s);
    }
    else {
      s = refEdge.points[ip].x();
      t = refEdge.points[ip].y();

      p = mapPointOnLine(p1, p2, s);
    }

    double l = std::hypot(p2.x() - p1.x(), p2.y() - p1.y());

    point = movePointPerpLine(p, edge.angle, l*t);
  }
  else {
    bool flip = false;

    bool symmetric = (edge.transform == EdgeTransform::SYMMETRIC ||
                      edge.transform == EdgeTransform::SYMMETRIC_MIRROR);
    bool mirror    = (edge.transform == EdgeTransform::SYMMETRIC_MIRROR);

    if (symmetric) {
      if (ip > np_/2) {
        ip   = np_/2 - (ip - np_/2);
        flip = true;
      }
    }

    double s = edge.points[ip].x();
    double t = edge.points[ip].y();

    if (flip && mirror)
      t = -t;

    QPointF p;

    if (flip)
      p = mapPointOnLine(p2, p1, s);
    else
      p = mapPointOnLine(p1, p2, s);

    double l = std::hypot(p2.x() - p1.x(), p2.y() - p1.y());

    point = movePointPerpLine(p, edge.angle, l*t);
  }

  if (! transform)
    return point;

  auto pr = rotatePoint(point, center_, angle_);

  auto ps = scalePoint(xscale_, yscale_, pr, center_);

  return ps;
}

void
CQTessel::
setPoint(int i, const QPointF &point)
{
  int ie = i / np_;
  int ip = i % np_;

  assert(ie >= 0 && ie < ne_ && ip >= 0 && ip <= np_);

  //---

  auto &edge = edges_[ie];

  auto p1 = scalePoint(xscale_, yscale_, edge.p1, center_);
  auto p2 = scalePoint(xscale_, yscale_, edge.p2, center_);

  //---

  auto point1 = rotatePoint(point, center_, -angle_);

  double s, t;

  pointToEdge(point1, p1, p2, s, t);

  //---

  bool mirrorParallel = false;
  bool mirrorPerp     = false;

  if (edge.type == EdgeType::REFERENCE) {
    mirrorParallel = (edge.transform == EdgeTransform::MIRROR_BOTH ||
                      edge.transform == EdgeTransform::MIRROR_PARALLEL);
    mirrorPerp     = (edge.transform == EdgeTransform::MIRROR_BOTH ||
                      edge.transform == EdgeTransform::MIRROR_PERP);

    auto &refEdge = edges_[edge.ind];

    if (mirrorParallel || mirrorPerp) {
      if (mirrorParallel) {
        ip = np_ - 1 - ip;
        s  = 1.0 - s;
      }

      if (mirrorPerp)
        t = -t;
    }

    refEdge.points[ip] = QPointF(s, t);
  }
  else {
    edge.points[ip] = QPointF(s, t);
  }
}

double
CQTessel::
deg2Rad(double a) const
{
  return M_PI*a/180.0;
}

void
CQTessel::
pointToEdge(const QPointF &point, const QPointF &p1, const QPointF &p2,
            double &s, double &t) const
{
  double dx1 = point.x() - p1.x();
  double dy1 = point.y() - p1.y();

  double dx2 = p2.x() - p1.x();
  double dy2 = p2.y() - p1.y();

  double u1 = dx1*dx2 + dy1*dy2; // square of distance of point to start
  double u2 = dx2*dx2 + dy2*dy2; // square of edge length

  assert(u2 > 0.0);

  s = u1/u2; // cos included angle

  double ix = p1.x() + s*dx2;
  double iy = p1.y() + s*dy2;

  t = std::hypot(point.x() - ix, point.y() - iy)/sqrt(u2);

  if (pointLineLeft(point, p1, p2))
    t = -t;
}

bool
CQTessel::
pointLineLeft(const QPointF &point, const QPointF &p1, const QPointF &p2) const
{
  auto triangleArea2 = [&](const QPointF &point1, const QPointF &point2, const QPointF &point3) {
    return (point2.x() - point1.x())*(point3.y() - point1.y()) -
           (point3.x() - point1.x())*(point2.y() - point1.y());
  };

  return triangleArea2(p1, p2, point) > 0.0;
}

QPointF
CQTessel::
rotatePoint(const QPointF &p, const QPointF &center, double da) const
{
  double da1 = deg2Rad(da);

  double s = std::sin(da1);
  double c = std::cos(da1);

  double x1 = p.x() - center.x();
  double y1 = p.y() - center.y();

  double x2 = x1*c - y1*s;
  double y2 = x1*s + y1*c;

  return QPointF(x2 + center.x(), y2 + center.y());
}

QPointF
CQTessel::
mapPointOnLine(const QPointF &p1, const QPointF &p2, double s) const
{
  auto lerp = [&](double x, double low, double high) {
    return low + (high - low)*x;
  };

  auto norm = [&](double x, double low, double high) {
    if (high != low)
      return (x - low)/(high - low);
    else
      return low;
  };

  auto map = [&](double value, double low1, double high1, double low2, double high2) {
    return lerp(norm(value, low1, high1), low2, high2);
  };

  //---

  double x = map(s, 0, 1, p1.x(), p2.x());
  double y = map(s, 0, 1, p1.y(), p2.y());

  return QPointF(x, y);
}

QPointF
CQTessel::
movePointOnLine(const QPointF &p, double a, double d) const
{
  double a1 = deg2Rad(a);

  double s = std::sin(a1);
  double c = std::cos(a1);

  return QPointF(p.x() + d*c, p.y() + d*s);
}

QPointF
CQTessel::
movePointPerpLine(const QPointF &p, double a, double d) const
{
  double a1 = deg2Rad(a);

  double s = std::sin(a1);
  double c = std::cos(a1);

  return QPointF(p.x() + d*s, p.y() - d*c);
}

QPointF
CQTessel::
scalePoint(double xscale, double yscale, const QPointF &p, const QPointF &center) const
{
  return QPointF(xscale*(p.x() - center.x()) + center.x(),
                 yscale*(p.y() - center.y()) + center.y());
}

QSize
CQTessel::
sizeHint() const
{
  return QSize(1000, 1000);
}

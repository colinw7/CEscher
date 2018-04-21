#include <CEscher.h>
#include <CMathGeom2D.h>
#include <CRGBUtil.h>
#include <CHSV.h>

CEscher::
CEscher()
{
}

CEscher::
~CEscher()
{
}

CPath2D *
CEscher::
createPath() const
{
  return new CPath2D;
}

void
CEscher::
clearShapes()
{
  shapes_.clear();
}

void
CEscher::
addShape(const CEscherShape &shape)
{
  shapes_.push_back(shape);
}

void
CEscher::
drawShape(const CEscherShape &shape)
{
  shape.draw(this);
}

//------

CEscherShape::
CEscherShape(CPath2D *path)
{
  if (path)
    paths_.push_back(new Path(path));

  transform_.setIdentity();
}

CEscherShape::
~CEscherShape()
{
  for (auto &path : paths_)
    delete path;
}

CEscherShape::
CEscherShape(const CEscherShape &shape) :
 transform_(shape.transform_), shapes_(shape.shapes_), rotateIndex_(shape.rotateIndex_)
{
  for (const auto &path : shape.paths_) {
    CPath2D *path1 = path->path->dup();

    paths_.push_back(new Path(path1, path->pen, path->brush));
  }
}

CEscherShape &
CEscherShape::
operator=(const CEscherShape &shape)
{
  for (auto &path : paths_)
    delete path;

  paths_.clear();

  for (const auto &path : shape.paths_) {
    CPath2D *path1 = path->path->dup();

    paths_.push_back(new Path(path1, path->pen, path->brush));
  }

  transform_   = shape.transform_;
  shapes_      = shape.shapes_;
  rotateIndex_ = shape.rotateIndex_;

  return *this;
}

CPoint2D
CEscherShape::
center() const
{
  CPoint2D c;

  transform_.multiplyPoint(CPoint2D(0.5, 0.5), c);

  return c;
}

void
CEscherShape::
addShape(const CEscherShape &shape)
{
  shapes_.push_back(shape);
}

void
CEscherShape::
setPenColor(int i, const CRGBA &c)
{
  paths_[i]->pen.setColor(c);
}

void
CEscherShape::
setPenWidth(int i, double w)
{
  paths_[i]->pen.setWidth(w);
}

void
CEscherShape::
setBrushColor(int i, const CRGBA &c)
{
  paths_[i]->brush.setStyle(CBRUSH_STYLE_SOLID);
  paths_[i]->brush.setColor(c);
}

CEscherShape
CEscherShape::
rotated(double a) const
{
  CEscherShape rshape = *this;

  CPoint2D c = center();

  //double xs = xSize();
  //double ys = ySize();

  double r = CMathGen::DegToRad(a);

  rshape.applyTransform(CMatrix2D::translation(-c.x, -c.y));
  rshape.applyTransform(CMatrix2D::rotation   (r));
  rshape.applyTransform(CMatrix2D::translation( c.x,  c.y));

  //double xs1 = xs*cos(r) + ys*sin(r);
  //double ys1 = xs*sin(r) + ys*cos(r);

  //rshape.setSize(xs1, ys1);

  return rshape;
}

CEscherShape
CEscherShape::
flipped() const
{
  CEscherShape rshape = *this;

  CPoint2D c = center();

  //double xs = xSize();
  //double ys = ySize();

  rshape.applyTransform(CMatrix2D::translation(-c.x, -c.y));
  rshape.applyTransform(CMatrix2D::scale      (-1.0, 1.0));
  rshape.applyTransform(CMatrix2D::translation( c.x,  c.y));

  return rshape;
}

CEscherShape
CEscherShape::
scaled(double s) const
{
  return scaled(s, s);
}

CEscherShape
CEscherShape::
scaled(double sx, double sy) const
{
  CEscherShape sshape = *this;

  CPoint2D c = center();

  //double xs = xSize();
  //double ys = ySize();

  sshape.applyTransform(CMatrix2D::translation(-c.x, -c.y));
  sshape.applyTransform(CMatrix2D::scale      (sx, sy));
  sshape.applyTransform(CMatrix2D::translation( c.x,  c.y));

  //double xs1 = xs*sx;
  //double ys1 = ys*sy;

  //sshape.setSize(xs1, ys1);

  return sshape;
}

CEscherShape
CEscherShape::
translated(double dx, double dy) const
{
  CEscherShape tshape = *this;

  tshape.applyTransform(CMatrix2D::translation(dx, dy));

  return tshape;
}

CEscherShape
CEscherShape::
flip(const CEscherShape &shape)
{
  return shape.flipped();
}

CEscherShape
CEscherShape::
rot(const CEscherShape &shape, double a)
{
  return shape.rotated(a);
}

CEscherShape
CEscherShape::
beside(const CEscherShape &lshape, const CEscherShape &rshape, double lf, double rf)
{
  double f = lf + rf;

  CEscherShape shape;

  CEscherShape lshape_s = lshape.scaled(lf/f, 1.0).translated(-0.5 + lf/(2*f), 0.0);
  CEscherShape rshape_s = rshape.scaled(rf/f, 1.0).translated( 0.5 - rf/(2*f), 0.0);

  shape.addShape(lshape_s);
  shape.addShape(rshape_s);

  return shape;
}

CEscherShape
CEscherShape::
above(const CEscherShape &tshape, const CEscherShape &bshape, double tf, double bf)
{
  double f = tf + bf;

  CEscherShape shape;

  CEscherShape tshape_s = tshape.scaled(1.0, tf/f).translated(0.0,  0.5 - tf/(2*f));
  CEscherShape bshape_s = bshape.scaled(1.0, bf/f).translated(0.0, -0.5 + bf/(2*f));

  shape.addShape(tshape_s);
  shape.addShape(bshape_s);

  return shape;
}

CEscherShape
CEscherShape::
over(const CEscherShape &shape1, const CEscherShape &shape2)
{
  CEscherShape shape;

  shape.addShape(shape1);
  shape.addShape(shape2);

  return shape;
}

CEscherShape
CEscherShape::
quartet(const CEscherShape &shape1, const CEscherShape &shape2,
        const CEscherShape &shape3, const CEscherShape &shape4)
{
  CEscherShape shape1c = shape1; shape1c.rotateColor( 1);
  CEscherShape shape4c = shape4; shape4c.rotateColor(-1);

  return above(beside(shape1c, shape2), beside(shape3, shape4c));
}

CEscherShape
CEscherShape::
nonet(const CEscherShape &shape1, const CEscherShape &shape2, const CEscherShape &shape3,
      const CEscherShape &shape4, const CEscherShape &shape5, const CEscherShape &shape6,
      const CEscherShape &shape7, const CEscherShape &shape8, const CEscherShape &shape9)
{
  return above(beside(shape1, beside(shape2, shape3), 1, 2),
               above(beside(shape4, beside(shape5, shape6), 1, 2),
                     beside(shape7, beside(shape8, shape9), 1, 2)),
               1, 2);
}

CEscherShape
CEscherShape::
recttri(const CEscherShape &shape, int n)
{
  if (n <= 0)
    return CEscherShape();

  return quartet(shape.rotated(), recttri(shape, n - 1),
                 recttri(shape, n - 1), recttri(shape, n - 1));
}

CEscherShape
CEscherShape::
rot45(const CEscherShape &shape)
{
  double ir2 = 1.0/sqrt(2.0);

  return over(shape.scaled(ir2).rotated(45).translated(0, 0.5), CEscherShape());
}

CEscherShape
CEscherShape::
side(const CEscherShape &shape, int n)
{
  if (n <= 0)
    return CEscherShape();

  return quartet(side(shape, n - 1), side(shape, n - 1), rot(shape), shape);
}

CEscherShape
CEscherShape::
corner(const CEscherShape &shape, int n)
{
  return corner(shape, shape, n);
}

CEscherShape
CEscherShape::
corner(const CEscherShape &tshape, const CEscherShape &ushape, int n)
{
  if (n <= 0)
    return CEscherShape();

  return quartet(corner(tshape, ushape, n - 1), side(tshape, n - 1),
                 rot(side(tshape, n - 1)), ushape);
}

CEscherShape
CEscherShape::
squarelimit(const CEscherShape &shape, int n)
{
  if (n <= 0)
    return shape;

  CEscherShape smallshape = flip(rot45(shape));

  CEscherShape tshape =
    over(shape, over(smallshape, rot(rot(rot(smallshape)))));

  CEscherShape ushape =
   over(over(over(smallshape, rot(smallshape)), rot(rot(smallshape))), rot(rot(rot(smallshape))));

  return squarelimit(tshape, ushape, n);
}

CEscherShape
CEscherShape::
squarelimit(const CEscherShape &tshape, const CEscherShape &ushape, int n)
{
  return nonet(corner(tshape, ushape, n),
               side(tshape, n),
               rot(rot(rot(corner(tshape, ushape, n)))),
               rot(side(tshape, n)),
               ushape,
               rot(rot(rot(side(tshape, n)))),
               rot(corner(tshape, ushape, n)),
               rot(rot(side(tshape, n))),
               rot(rot(corner(tshape, ushape, n))));
}

void
CEscherShape::
applyTransform(const CMatrix2D &m)
{
  transform_ = m*transform_;
}

void
CEscherShape::
draw(CEscher *escher) const
{
  CMatrix2D transform;

  transform.setIdentity();

  draw(escher, transform);
}

void
CEscherShape::
draw(CEscher *escher, const CMatrix2D &transform) const
{
  CMatrix2D transform1 = transform*transform_;

  if (escher->isShowBorder()) {
    CPen pen;

    pen.setColor(CRGBA(0,0,0));
    pen.setWidth(0.0005);

    escher->setPen(pen);

    CPoint2D p1, p2, p3, p4;

    transform1.multiplyPoint(CPoint2D(0, 0), p1);
    transform1.multiplyPoint(CPoint2D(1, 0), p2);
    transform1.multiplyPoint(CPoint2D(1, 1), p3);
    transform1.multiplyPoint(CPoint2D(0, 1), p4);

    escher->drawLine(p1, p2);
    escher->drawLine(p2, p3);
    escher->drawLine(p3, p4);
    escher->drawLine(p4, p1);
  }

  //---

  for (const auto &path : paths_) {
    if (! path->tpath) {
      Path *mpath = const_cast<Path *>(path);

      mpath->tpath = mpath->path->dup();

      mpath->tpath->transform(transform1);
    }

    if (path->brush.isValid()) {
      escher->setBrush(path->brush);

      escher->fill(path->tpath);
    }

    if (path->pen.isValid()) {
      escher->setPen(path->pen);

      escher->stroke(path->tpath);
    }
  }

  for (const auto &shape : shapes_) {
    shape.draw(escher, transform1);
  }
}

void
CEscherShape::
rotateColor(int n)
{
  if (rotateIndex_ >= 0) {
    CRGBA c = paths_[rotateIndex_]->brush.getColor();

    CHSV c1 = CRGBUtil::RGBtoHSV(c.getRGB());

    c1.setHue(c1.getHue() + n*60);

    c = CRGBUtil::HSVtoRGB(c1);

    paths_[rotateIndex_]->brush.setColor(c);
  }

  for (auto &shape : shapes_)
    shape.rotateColor(n);
}

//------

CEscherFShape::
CEscherFShape(CEscher *escher)
{
  CPath2D *path = escher->createPath();

  path->moveTo(CPoint2D(0.1, 0.1));
  path->lineTo(CPoint2D(0.1, 0.9));
  path->lineTo(CPoint2D(0.9, 0.9));
  path->lineTo(CPoint2D(0.9, 0.8));
  path->lineTo(CPoint2D(0.2, 0.8));
  path->lineTo(CPoint2D(0.2, 0.6));
  path->lineTo(CPoint2D(0.6, 0.6));
  path->lineTo(CPoint2D(0.6, 0.5));
  path->lineTo(CPoint2D(0.2, 0.5));
  path->lineTo(CPoint2D(0.2, 0.1));
  path->close ();

  paths_.push_back(new Path(path));
}

CEscherTriangleShape::
CEscherTriangleShape(CEscher *escher)
{
  CPath2D *path = escher->createPath();

  path->moveTo(CPoint2D(0, 0));
  path->lineTo(CPoint2D(0, 1));
  path->lineTo(CPoint2D(1, 0));
  path->close ();

  paths_.push_back(new Path(path));
}

CEscherEdgeShape::
CEscherEdgeShape(CEscher *escher)
{
  CPath2D *path = escher->createPath();

  path->moveTo   (CPoint2D(1.00, 0.00));
  path->bezier3To(CPoint2D(0.78, 0.05), CPoint2D(0.78, 0.05), CPoint2D(0.58, 0.03));
  path->bezier3To(CPoint2D(0.38, 0.20), CPoint2D(0.38, 0.20), CPoint2D(0.25, 0.27));
  path->lineTo   (CPoint2D(0.00, 0.00));

  paths_.push_back(new Path(path));
}

CEscherFishShape::
CEscherFishShape(CEscher *escher)
{
  CPath2D *path1 = escher->createPath();

  path1->moveTo   (CPoint2D( 0.00, 1.00));
  path1->bezier3To(CPoint2D( 0.08, 0.98), CPoint2D( 0.22, 0.82), CPoint2D( 0.29, 0.72));
  path1->moveTo   (CPoint2D( 0.30, 0.50));
  path1->bezier3To(CPoint2D( 0.34, 0.40), CPoint2D( 0.43, 0.32), CPoint2D( 0.50, 0.26));
  path1->moveTo   (CPoint2D( 0.50, 0.26));
  path1->bezier3To(CPoint2D( 0.58, 0.21), CPoint2D( 0.66, 0.22), CPoint2D( 0.76, 0.20));
  path1->moveTo   (CPoint2D( 0.76, 0.20));
  path1->bezier3To(CPoint2D( 0.82, 0.12), CPoint2D( 0.94, 0.05), CPoint2D( 1.00, 0.00));
  path1->moveTo   (CPoint2D( 1.00, 0.00));
  path1->bezier3To(CPoint2D( 0.90, 0.03), CPoint2D( 0.81, 0.04), CPoint2D( 0.76, 0.05));
  path1->moveTo   (CPoint2D( 0.76, 0.05));
  path1->bezier3To(CPoint2D( 0.69, 0.04), CPoint2D( 0.62, 0.04), CPoint2D( 0.55, 0.04));
  path1->moveTo   (CPoint2D( 0.55, 0.04));
  path1->bezier3To(CPoint2D( 0.49, 0.10), CPoint2D( 0.40, 0.17), CPoint2D( 0.35, 0.20));
  path1->moveTo   (CPoint2D( 0.35, 0.20));
  path1->bezier3To(CPoint2D( 0.29, 0.24), CPoint2D( 0.19, 0.28), CPoint2D( 0.14, 0.31));
  path1->moveTo   (CPoint2D( 0.14, 0.31));
  path1->bezier3To(CPoint2D( 0.09, 0.35), CPoint2D(-0.03, 0.43), CPoint2D(-0.05, 0.72));
  path1->moveTo   (CPoint2D(-0.05, 0.72));
  path1->bezier3To(CPoint2D(-0.04, 0.82), CPoint2D(-0.02, 0.95), CPoint2D( 0.00, 1.00));
  path1->moveTo   (CPoint2D( 0.10, 0.85));
  path1->bezier3To(CPoint2D( 0.14, 0.82), CPoint2D( 0.18, 0.78), CPoint2D( 0.18, 0.75));
  path1->moveTo   (CPoint2D( 0.18, 0.75));
  path1->bezier3To(CPoint2D( 0.16, 0.74), CPoint2D( 0.14, 0.73), CPoint2D( 0.12, 0.73));
  path1->moveTo   (CPoint2D( 0.12, 0.73));
  path1->bezier3To(CPoint2D( 0.11, 0.77), CPoint2D( 0.11, 0.81), CPoint2D( 0.10, 0.85));
  path1->moveTo   (CPoint2D( 0.05, 0.82));
  path1->bezier3To(CPoint2D( 0.10, 0.80), CPoint2D( 0.08, 0.74), CPoint2D( 0.09, 0.70));
  path1->moveTo   (CPoint2D( 0.09, 0.70));
  path1->bezier3To(CPoint2D( 0.07, 0.68), CPoint2D( 0.06, 0.66), CPoint2D( 0.04, 0.67));
  path1->moveTo   (CPoint2D( 0.04, 0.67));
  path1->bezier3To(CPoint2D( 0.04, 0.73), CPoint2D( 0.04, 0.81), CPoint2D( 0.05, 0.82));
  path1->moveTo   (CPoint2D( 0.11, 0.70));
  path1->bezier3To(CPoint2D( 0.16, 0.56), CPoint2D( 0.24, 0.39), CPoint2D( 0.30, 0.34));
  path1->moveTo   (CPoint2D( 0.30, 0.34));
  path1->bezier3To(CPoint2D( 0.41, 0.22), CPoint2D( 0.62, 0.16), CPoint2D( 0.80, 0.08));
  path1->moveTo   (CPoint2D( 0.23, 0.80));
  path1->bezier3To(CPoint2D( 0.35, 0.80), CPoint2D( 0.44, 0.78), CPoint2D( 0.50, 0.75));
  path1->moveTo   (CPoint2D( 0.50, 0.75));
  path1->bezier3To(CPoint2D( 0.50, 0.67), CPoint2D( 0.50, 0.59), CPoint2D( 0.50, 0.51));
  path1->moveTo   (CPoint2D( 0.50, 0.51));
  path1->bezier3To(CPoint2D( 0.46, 0.47), CPoint2D( 0.42, 0.43), CPoint2D( 0.38, 0.39));
  path1->moveTo   (CPoint2D( 0.29, 0.71));
  path1->bezier3To(CPoint2D( 0.36, 0.74), CPoint2D( 0.43, 0.73), CPoint2D( 0.48, 0.69));
  path1->moveTo   (CPoint2D( 0.34, 0.61));
  path1->bezier3To(CPoint2D( 0.38, 0.66), CPoint2D( 0.44, 0.64), CPoint2D( 0.48, 0.63));
  path1->moveTo   (CPoint2D( 0.34, 0.51));
  path1->bezier3To(CPoint2D( 0.38, 0.56), CPoint2D( 0.41, 0.58), CPoint2D( 0.48, 0.57));
  path1->moveTo   (CPoint2D( 0.45, 0.42));
  path1->bezier3To(CPoint2D( 0.46, 0.40), CPoint2D( 0.47, 0.39), CPoint2D( 0.48, 0.39));
  path1->moveTo   (CPoint2D( 0.42, 0.39));
  path1->bezier3To(CPoint2D( 0.43, 0.36), CPoint2D( 0.46, 0.32), CPoint2D( 0.48, 0.33));
  path1->moveTo   (CPoint2D( 0.25, 0.26));
  path1->bezier3To(CPoint2D( 0.17, 0.17), CPoint2D( 0.08, 0.09), CPoint2D( 0.00, 0.01));
  path1->moveTo   (CPoint2D( 0.00, 0.01));
  path1->bezier3To(CPoint2D(-0.08, 0.09), CPoint2D(-0.17, 0.18), CPoint2D(-0.25, 0.26));
  path1->moveTo   (CPoint2D(-0.25, 0.26));
  path1->bezier3To(CPoint2D(-0.20, 0.37), CPoint2D(-0.11, 0.47), CPoint2D(-0.03, 0.57));
  path1->moveTo   (CPoint2D(-0.17, 0.26));
  path1->bezier3To(CPoint2D(-0.13, 0.34), CPoint2D(-0.08, 0.40), CPoint2D(-0.01, 0.44));
  path1->moveTo   (CPoint2D(-0.12, 0.21));
  path1->bezier3To(CPoint2D(-0.07, 0.29), CPoint2D(-0.02, 0.34), CPoint2D( 0.05, 0.40));
  path1->moveTo   (CPoint2D(-0.06, 0.14));
  path1->bezier3To(CPoint2D(-0.03, 0.23), CPoint2D( 0.03, 0.28), CPoint2D( 0.10, 0.34));
  path1->moveTo   (CPoint2D(-0.02, 0.08));
  path1->bezier3To(CPoint2D( 0.02, 0.16), CPoint2D( 0.09, 0.23), CPoint2D( 0.16, 0.30));

  //---

  auto p2_1 = [](double x, double y) -> CPoint2D {
    return CPoint2D(x/100.0, -y/100.0);
  };
  auto p2_2 = [](double x, double y) -> CPoint2D {
    return CPoint2D((x + 2.2267213)/100.0, 1.0 - (y + 97.682502)/100.0);
  };

  CPath2D *path2 = escher->createPath();

  path2->moveTo    (p2_2(- 2.0271207,-97.66951900));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(- 1.9090713,  5.78486900),
                    p2_1(- 2.9140311, 11.86517500));
  path2->rbezier3To(p2_1(- 1.0049599,  6.08030700),
                    p2_1(- 2.1419039, 16.06739200),
                    p2_1(- 2.1419039, 16.06739200));
  path2->rlineTo   (p2_1(  0.7576066,  7.50254100));
  path2->rlineTo   (p2_1(  1.3882155,  7.29233800));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(- 4.4834351,  5.87798100),
                    p2_1(-11.4067384, 14.34312700));
  path2->rbezier3To(p2_1(- 6.1444220,  7.51280600),
                    p2_1(-10.6843970, 16.88031500),
                    p2_1(-10.6843970, 16.88031500));
  path2->lineTo    (p2_2(- 2.0690691,  1.23006640));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(  7.6577310,- 7.43417240),
                    p2_1( 13.1125641,-12.69100540));
  path2->rbezier3To(p2_1(  5.4548330,- 5.25683300),
                    p2_1( 11.5604730,-11.83221600),
                    p2_1( 11.5604730,-11.83221600));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(  7.7073150,  3.68173900),
                    p2_1( 14.5215010,  8.45748800));
  path2->rbezier3To(p2_1(  6.8141860,  4.77574900),
                    p2_1( 15.7615590, 13.11033270),
                    p2_1( 15.7615590, 13.11033270));
  path2->rbezier3To(p2_1(  0.0507200,- 0.02536100),
                    p2_1(  6.1911950,  0.10726070),
                    p2_1( 10.6391090,- 0.19234270));
  path2->rbezier3To(p2_1(  4.4479140,- 0.29960340),
                    p2_1( 10.4210140,- 0.77044600),
                    p2_1( 10.4210140,- 0.77044600));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(  8.3347690,  1.27500750),
                    p2_1( 12.3561130,  2.11617963));
  path2->rbezier3To(p2_1(  4.0213440,  0.84117216),
                    p2_1( 11.7589650,  2.91237707),
                    p2_1( 11.7589650,  2.91237707));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(- 9.0724640,- 6.57415150),
                    p2_1(-14.6819380,-10.86498470));
  path2->rbezier3To(p2_1(- 5.6094730,- 4.29083300),
                    p2_1(- 9.5767950,- 9.16528300),
                    p2_1(- 9.5767950,- 9.16528300));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(- 4.9186290,- 0.83448600),
                    p2_1(-10.9861230,- 1.46759600));
  path2->rbezier3To(p2_1(- 5.8690090,- 0.61239900),
                    p2_1(-12.7495600,- 2.77252600),
                    p2_1(-16.0878720,- 5.48438800));
  path2->bezier3To (p2_2( 41.6547190,-28.76429500),
                    p2_2( 35.1557600,-36.08142500),
                    p2_2( 35.1557600,-36.08142500));
  path2->rlineTo   (p2_1( 12.6490580,-12.53612000));
  path2->rlineTo   (p2_1(  0.0000000,-24.50754800));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(-10.5581450,- 3.37193800),
                    p2_1(-15.2466320,- 3.95283000));
  path2->rbezier3To(p2_1(- 9.1127200,- 1.12904400),
                    p2_1(-11.4028320,- 0.09639000),
                    p2_1(-11.7455530,- 0.56469000));
  path2->rbezier3To(p2_1(  0.0000000,  0.00000000),
                    p2_1(- 7.7570080,- 8.85051600),
                    p2_1(-10.7738250,-11.70867500));
  path2->rbezier3To(p2_1(- 8.0228277,- 7.60089600),
                    p2_1(-12.0659287,- 8.31823100),
                    p2_1(-12.0659287,- 8.31823100));
  path2->close     ();


  //---

  auto p3_1 = [](double x, double y) -> CPoint2D {
    return CPoint2D(x/100.0, -y/100.0);
  };
  auto p3_2 = [](double x, double y) -> CPoint2D {
    return CPoint2D((x + 2.2267213)/100.0, 1.0 - (y + 97.682502)/100.0);
  };

  CPath2D *path3 = escher->createPath();

  path3->moveTo    (p3_2( 7.9483020,-82.6381610));
  path3->rbezier3To(p3_1( 0.5646900,  3.0210910),
                    p3_1( 0.8254643,  4.3359820),
                    p3_1( 1.0668031,  6.5842280));
  path3->rbezier3To(p3_1( 0.2592964,  2.4155330),
                    p3_1( 0.9554786,  5.3632870),
                    p3_1( 0.9554786,  5.3632870));
  path3->rbezier3To(p3_1( 0.0000000,  0.0000000),
                    p3_1( 1.9731033,- 0.1838450),
                    p3_1( 3.2752023,- 0.7403770));
  path3->rbezier3To(p3_1( 1.3020990,- 0.5565320),
                    p3_1( 2.6999110,- 1.2607570),
                    p3_1( 2.6999110,- 1.2607570));
  path3->rbezier3To(p3_1( 0.0000000,  0.0000000),
                    p3_1(-0.5585410,- 3.3928520),
                    p3_1(-3.1516630,- 5.7386330));
  path3->rbezier3To(p3_1(-2.6831660,- 2.4272370),
                    p3_1(-4.8457320,- 4.2077480),
                    p3_1(-4.8457320,- 4.2077480));
  path3->close     ();

  //---

  auto p4_1 = [](double x, double y) -> CPoint2D {
    return CPoint2D(x/100.0, -y/100.0);
  };
  auto p4_2 = [](double x, double y) -> CPoint2D {
    return CPoint2D((x + 2.2267213)/100.0, 1.0 - (y + 97.682502)/100.0);
  };

  CPath2D *path4 = escher->createPath();

  path4->moveTo    (p4_2( 2.9002033,-79.641385));
  path4->rbezier3To(p4_1( 0.0000000,  0.000000),
                    p4_1(-0.4816311,  0.916527),
                    p4_1(-0.6439085,  2.734427));
  path4->rbezier3To(p4_1(-0.1353091,  1.515788),
                    p4_1(-0.3205060,  3.275952),
                    p4_1(-0.3043097,  6.040625));
  path4->rbezier3To(p4_1( 0.0323930,  5.529347),
                    p4_1(-0.0037800,  6.127179),
                    p4_1(-0.0037800,  6.127179));
  path4->rbezier3To(p4_1( 0.0000000,  0.000000),
                    p4_1( 0.6316277,  0.839714),
                    p4_1( 2.5755444,- 0.335994));
  path4->rbezier3To(p4_1( 0.6187832,- 0.374249),
                    p4_1( 2.3712372,- 2.568533),
                    p4_1( 2.3712372,- 2.568533));
  path4->rbezier3To(p4_1( 0.0000000,  0.000000),
                    p4_1(-0.2950391,- 4.282969),
                    p4_1(-0.4858639,- 7.007227));
  path4->rbezier3To(p4_1(-0.2910269,- 4.154768),
                    p4_1(-3.5089149,- 4.990477),
                    p4_1(-3.5089195,- 4.990477));
  path4->close     ();

  //---

  CPath2D *path5 = escher->createPath();

  path5->ellipse(p4_2(4.3232102, -68.234818), 0.02, 0.02);

  //---

  CPath2D *path6 = escher->createPath();

  path6->ellipse(p4_2(12.00754, -73.492516), 0.02, 0.02);

  //---

  paths_.push_back(new Path(path2));
  paths_.push_back(new Path(path3));
  paths_.push_back(new Path(path4));
  paths_.push_back(new Path(path5));
  paths_.push_back(new Path(path6));
  paths_.push_back(new Path(path1));
}

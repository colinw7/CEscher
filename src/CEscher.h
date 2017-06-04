#ifndef CEscher_H
#define CEscher_H

#include <CPath2D.h>
#include <CMatrix2D.h>
#include <CBrush.h>
#include <CPen.h>

class CEscher;

class CEscherShape {
 public:
  CEscherShape(CPath2D *path=nullptr);

  CEscherShape(const CEscherShape &shape);

  CEscherShape &operator=(const CEscherShape &shape);

  virtual ~CEscherShape();

  CPoint2D center() const;

  const CPen &pen(int i) const { return paths_[i]->pen; }
  void setPen(int i, const CPen &p) { paths_[i]->pen = p; }

  const CBrush &brush(int i) const { return paths_[i]->brush; }
  void setBrush(int i, const CBrush &b) { paths_[i]->brush = b; }

  void setPenColor(int i, const CRGBA &c);
  void setPenWidth(int i, double w);

  void setBrushColor(int i, const CRGBA &c);

  int rotateIndex() const { return rotateIndex_; }
  void setRotateIndex(int i) { rotateIndex_ = i; }

  CEscherShape rotated(double a=90.0) const;
  CEscherShape flipped() const;
  CEscherShape scaled (double s) const;
  CEscherShape scaled (double sx, double sy) const;
  CEscherShape translated(double dx, double dy) const;

  static CEscherShape rot (const CEscherShape &shape, double a=90.0);
  static CEscherShape flip(const CEscherShape &shape0);

  static CEscherShape beside(const CEscherShape &lshape, const CEscherShape &rshape,
                             double lf=1.0, double rf=1.0);
  static CEscherShape above (const CEscherShape &tshape, const CEscherShape &bshape,
                            double tf=1.0, double bf=1.0);
  static CEscherShape over  (const CEscherShape &shape1, const CEscherShape &shape2);

  static CEscherShape quartet(const CEscherShape &shape1, const CEscherShape &shape2,
                              const CEscherShape &shape3, const CEscherShape &shape4);

  static CEscherShape nonet(const CEscherShape &shape1, const CEscherShape &shape2,
                            const CEscherShape &shape3, const CEscherShape &shape4,
                            const CEscherShape &shape5, const CEscherShape &shape6,
                            const CEscherShape &shape7, const CEscherShape &shape8,
                            const CEscherShape &shape9);

  static CEscherShape recttri(const CEscherShape &shape, int n);

  static CEscherShape rot45(const CEscherShape &shape);

  static CEscherShape side(const CEscherShape &shape, int n);

  static CEscherShape corner(const CEscherShape &shape, int n);
  static CEscherShape corner(const CEscherShape &tshape, const CEscherShape &ushape, int n);

  static CEscherShape squarelimit(const CEscherShape &shape, int n);
  static CEscherShape squarelimit(const CEscherShape &tshape, const CEscherShape &ushape, int n);

  void applyTransform(const CMatrix2D &m);

  void addShape(const CEscherShape &shape);

  void draw(CEscher *escher) const;

  void rotateColor(int n=1);

 private:
  void draw(CEscher *escher, const CMatrix2D &transform) const;

 protected:
  class Path {
   public:
    CPath2D *path { nullptr };
    CPen     pen;
    CBrush   brush;
    CPath2D *tpath { nullptr };

    Path(CPath2D *path) :
     path(path) {
      brush.setStyle(CBRUSH_STYLE_NONE);

      pen.setWidth(0.001);
    }

    Path(CPath2D *path, const CPen &pen, const CBrush &brush) :
     path(path), pen(pen), brush(brush) {
    }

   ~Path() {
      delete path;
      delete tpath;
    }

   private:
    Path(const Path &path);
    Path &operator=(const Path &path);
  };

  typedef std::vector<CEscherShape> Shapes;
  typedef std::vector<Path *>       Paths;

  Paths     paths_;
  CMatrix2D transform_;
  Shapes    shapes_;
  int       rotateIndex_ { -1 };
};

//---

class CEscherFShape : public CEscherShape {
 public:
  CEscherFShape(CEscher *escher);
};

//---

class CEscherTriangleShape : public CEscherShape {
 public:
  CEscherTriangleShape(CEscher *escher);
};

//---

class CEscherFishShape : public CEscherShape {
 public:
  CEscherFishShape(CEscher *escher);
};

//---

class CEscherEdgeShape : public CEscherShape {
 public:
  CEscherEdgeShape(CEscher *escher);
};

//---

class CEscher {
 public:
  typedef std::vector<CEscherShape> Shapes;

 public:
  CEscher();

  virtual ~CEscher();

  bool isShowBorder() const { return showBorder_; }
  void setShowBorder(bool b) { showBorder_ = b; }

  int iterations() const { return iterations_; }
  void setIterations(int i) { iterations_ = i; }

  virtual CPath2D *createPath() const;

  const Shapes &shapes() const { return shapes_; }

  void clearShapes();

  void addShape(const CEscherShape &shape);

  void drawShape(const CEscherShape &shape);

  virtual void setPen(const CPen &pen) = 0;
  virtual void setBrush(const CBrush &brush) = 0;

  virtual void drawLine(const CPoint2D &p1, const CPoint2D &p2) = 0;

  virtual void fill(CPath2D *path) = 0;
  virtual void stroke(CPath2D *path) = 0;

 private:
  bool   showBorder_ { false };
  int    iterations_ { 3 };
  Shapes shapes_;
};

#endif

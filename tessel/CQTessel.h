#ifndef CQTessel_H
#define CQTessel_H

#include <CDisplayRange2D.h>

#include <QFrame>

class CQTessel;
class CQPropertyViewTree;

class CQTesselApp : public QFrame {
  Q_OBJECT

 public:
  CQTesselApp(QWidget *parent=nullptr);

 private:
  CQTessel*           tessel_   { nullptr };
  CQPropertyViewTree* viewTree_ { nullptr };
};

class CQTessel : public QFrame {
  Q_OBJECT

  Q_PROPERTY(Style   style    READ style    WRITE setStyle   )
  Q_PROPERTY(Pattern pattern  READ pattern  WRITE setPattern )
  Q_PROPERTY(QPointF center   READ center   WRITE setCenter  )
  Q_PROPERTY(double  radius   READ radius   WRITE setRadius  )
  Q_PROPERTY(double  angle    READ angle    WRITE setAngle   )
  Q_PROPERTY(double  xscale   READ xscale   WRITE setXScale  )
  Q_PROPERTY(double  yscale   READ yscale   WRITE setYScale  )
  Q_PROPERTY(int     numEdge  READ numEdge  WRITE setNumEdge )
  Q_PROPERTY(int     gridSize READ gridSize WRITE setGridSize)

  Q_ENUMS(Style)
  Q_ENUMS(Pattern)

 public:
  enum class Style {
    TRIANGLE,
    SQUARE,
    HEXAGON
  };

  enum class Pattern {
    OPPOSITE,
    ADJACENT,
    REPEAT_ALL,
    TWO_TILE
  };

  CQTessel(QWidget *parent=nullptr);

  void resetPoints();

  //---

  const Style &style() const { return style_; }
  void setStyle(const Style &s) { style_ = s; resetPoints(); update(); }

  const Pattern &pattern() const { return pattern_; }
  void setPattern(const Pattern &p) { pattern_ = p; resetPoints(); update(); }

  const QPointF &center() const { return center_; }
  void setCenter(const QPointF &v) { center_ = v; update(); }

  double radius() const { return radius_; }
  void setRadius(double r) { radius_ = r; update(); }

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; update(); }

  double xscale() const { return xscale_; }
  void setXScale(double r) { xscale_ = r; update(); }

  double yscale() const { return yscale_; }
  void setYScale(double r) { yscale_ = r; update(); }

  int numEdge() const { return numEdge_; }
  void setNumEdge(int n) { numEdge_ = n; resetPoints(); update(); }

  int gridSize() const { return gridSize_; }
  void setGridSize(int n) { gridSize_ = n; update(); }

  //---

  struct TileBox {
    double dx       { 0.0 };
    double dy       { 0.0 };
    double angle    { 0.0 };
    bool   visible  { true };
    int    colorInd { 0 };

    TileBox() = default;

    TileBox(double dx, double dy, double angle=0.0, int colorInd=0) :
     dx(dx), dy(dy), angle(angle), colorInd(colorInd) {
    }
  };

  using TileBoxes = std::vector<TileBox>;

  TileBox tileBox(int row, int col) const;

  //---

  QPointF windowToPixel(const QPointF &w) const;
  QPointF pixelToWindow(const QPointF &p) const;

  //---

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void keyPressEvent(QKeyEvent *e) override;

  //---

  void updateMousePos(const QPoint &pos);
  void updateNearest (const QPoint &pos);

  int pointInd(const QPointF &p) const;

  QPointF getPoint(int i, bool transform=true) const;
  void setPoint(int i, const QPointF &p);

  QPointF rotatePoint(const QPointF &p, const QPointF &center, double da) const;

  double deg2Rad(double a) const;

  void pointToEdge(const QPointF &point, const QPointF &p1, const QPointF &p2,
                   double &s, double &t) const;

  bool pointLineLeft(const QPointF &point, const QPointF &p1, const QPointF &p2) const;

  QPointF mapPointOnLine(const QPointF &p1, const QPointF &p2, double s) const;

  QPointF movePointOnLine  (const QPointF &p, double a, double d) const;
  QPointF movePointPerpLine(const QPointF &p, double a, double d) const;

  QPointF scalePoint(double xscale, double yscale, const QPointF &p, const QPointF &center) const;

  QSize sizeHint() const override;

 private:
  using Points = std::vector<QPointF>;

  enum class EdgeType {
    NONE,
    POINTS,
    REFERENCE
  };

  enum class EdgeTransform {
    NONE,
    MIRROR_BOTH,
    MIRROR_PARALLEL,
    MIRROR_PERP,
    SYMMETRIC,
    SYMMETRIC_MIRROR
  };

  struct Edge {
    EdgeType      type      { EdgeType::NONE };
    EdgeTransform transform { EdgeTransform::NONE };
    int           ind       { -1 };
    double        angle     { 0 };
    QPointF       p1, p2;
    Points        points;
  };

  using Edges = std::vector<Edge>;

  struct MouseData {
    QPointF         pos;
    int             ind        { 0 };
    bool            pressed    { false };
    Qt::MouseButton button     { Qt::LeftButton };
    QPointF         pressPoint;
    QPointF         edgePos;

    void reset() { ind = 0; }
  };

  Style           style_      { Style::SQUARE };
  Pattern         pattern_    { Pattern::OPPOSITE };
  CDisplayRange2D range_;
  Edges           edges_;
  int             ne_         { 0 };
  int             np_         { 0 };
  int             npe_        { 0 };
  QPointF         center_     { 0.0, 0.0 };
  double          radius_     { 1.0 };
  double          angle_      { 0.0 };
  double          xscale_     { 1.0 };
  double          yscale_     { 1.0 };
  int             numEdge_    { 7 }; // always odd for symmetry ?
  int             gridSize_   { 2 };
  MouseData       mouseData_;
};

#endif

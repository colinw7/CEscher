#ifndef CQLizard_H
#define CQLizard_H

#include <CDisplayRange2D.h>

#include <QFrame>

class CQLizard;
class CQPropertyViewTree;

class CQLizardApp : public QFrame {
  Q_OBJECT

 public:
  CQLizardApp(QWidget *parent=nullptr);

 private:
  CQLizard*           lizard_   { nullptr };
  CQPropertyViewTree* viewTree_ { nullptr };
};

class CQLizard : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QPointF center  READ center  WRITE setCenter )
  Q_PROPERTY(double  radius  READ radius  WRITE setRadius )
  Q_PROPERTY(QPointF rcenter READ rcenter WRITE setRCenter)
  Q_PROPERTY(double  rangle  READ rangle  WRITE setRAngle )

 public:
  CQLizard(QWidget *parent=nullptr);

  void lizardPoints();
  void resetPoints();

  const QPointF &center() const { return center_; }
  void setCenter(const QPointF &v) { center_ = v; update(); }

  double radius() const { return radius_; }
  void setRadius(double r) { radius_ = r; update(); }

  const QPointF &rcenter() const { return rcenter_; }
  void setRCenter(const QPointF &v) { rcenter_ = v; update(); }

  double rangle() const { return rangle_; }
  void setRAngle(double a) { rangle_ = a; update(); }

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void mousePressEvent  (QMouseEvent *me) override;
  void mouseMoveEvent   (QMouseEvent *me) override;
  void mouseReleaseEvent(QMouseEvent *me) override;

  void updateMousePos(const QPoint &pos);

  int pointInd(const QPointF &p) const;

  void keyPressEvent(QKeyEvent *e) override;

  QSize sizeHint() const override;

 private:
  using Points = std::vector<QPointF>;

  CDisplayRange2D range_;
  Points          points_;
  QPointF         center_   { 0.0, 0.0 };
  QPointF         rcenter_  { 0.0, 0.0 };
  double          rangle_   { 30.0 };
  double          radius_   { 1.0 };
  double          dx_[7];
  double          dy_[7];
  QPointF         mousePos_;
  int             mouseInd_ { 0 };
  bool            pressed_  { false };
  Qt::MouseButton button_   { Qt::LeftButton };
};

#endif

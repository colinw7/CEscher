#ifndef CQEscher_H
#define CQEscher_H

#include <CEscher.h>
#include <CDisplayRange2D.h>

#include <QFrame>

class CQEscher;
class CQPath2D;

class QCheckBox;
class QSpinBox;
class QPainter;

class CQEscherApp : public QFrame {
  Q_OBJECT

 public:
  CQEscherApp(QWidget *parent=nullptr);

  void saveSVG(const QString &filename) const;

 private slots:
  void updateStateLater();
  void updateState();

 private:
  CQEscher*  escher_     { nullptr };
  QCheckBox* borders_    { nullptr };
  QSpinBox*  iterations_ { nullptr };
};

//---

class CQEscher : public QFrame, public CEscher {
  Q_OBJECT

  Q_PROPERTY(bool showBorder READ isShowBorder WRITE setShowBorder)
  Q_PROPERTY(int  iterations READ iterations   WRITE setIterations)

 public:
  CQEscher(QWidget *parent=0);

  void addShapes();

  CPath2D *createPath() const override;

  void resizeEvent(QResizeEvent *) override;

  void paintEvent(QPaintEvent *) override;

  void setPen(const CPen &pen) override;
  void setBrush(const CBrush &brush) override;

  void drawLine(const CPoint2D &p1, const CPoint2D &p2) override;

  void stroke(CPath2D *path) override;
  void fill(CPath2D *path) override;

  QSize sizeHint() const override { return QSize(800, 800); }

  void saveSVG(const QString &filename) const;

  CPoint2D transformPoint(const CPoint2D &p) const;

 private:
  QString pathToSVGString(CQPath2D *path) const;

 private:
  CDisplayRange2D range_;
  QPainter*       painter_ { nullptr };
  mutable FILE*   svgFp_ { nullptr };
  mutable CRGBA   svgBg_;
  mutable CRGBA   svgFg_;
  mutable int     svgSize_ { 1024 };
};

#endif

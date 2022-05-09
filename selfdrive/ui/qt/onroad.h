#pragma once

#include <QStackedLayout>
#include <QWidget>
#include <QPushButton>

#include "selfdrive/common/util.h"
#include "selfdrive/ui/qt/widgets/cameraview.h"
#include "selfdrive/ui/ui.h"

#ifdef QCOM2
#include <QTimer>
#include <QMap>
#include "selfdrive/ui/qt/screenrecorder/screenrecorder.h"
#endif


// ***** onroad widgets *****

class ButtonsWindow : public QWidget {
  Q_OBJECT

public:
  ButtonsWindow(QWidget* parent = 0);

private:
  QPushButton *dlpBtn;

  //int dlpStatus = -1;
  const QStringList dlpBtnColors = {"#87ceeb", "#00ff16", "#808080"};

public slots:
  void updateState(const UIState &s);
};

class OnroadHud : public QWidget {
  Q_OBJECT
  Q_PROPERTY(bool engageable MEMBER engageable NOTIFY valueChanged); 
  Q_PROPERTY(int status MEMBER status NOTIFY valueChanged);
  Q_PROPERTY(float ang_str MEMBER ang_str NOTIFY valueChanged);

public:
  explicit OnroadHud(QWidget *parent);
  void updateState(const UIState &s);

private:
  void drawIcon(QPainter &p, int x, int y, QPixmap &img, QBrush bg, float opacity, bool rotation = false, float angle = 0 );
  //void drawText(QPainter &p, int x, int y, const QString &text, int alpha = 255);
  //void drawTextWithColor(QPainter &p, int x, int y, const QString &text, QColor& color);
  void paintEvent(QPaintEvent *event) override;
  
  QPixmap engage_img;
  const int radius = 192;
  const int img_size = (radius / 2) * 1.5;
  bool engageable = false;
  int status = STATUS_DISENGAGED;
  float ang_str = 0;
  
signals:
  void valueChanged();
};

class OnroadAlerts : public QWidget {
  Q_OBJECT

public:
  OnroadAlerts(QWidget *parent = 0) : QWidget(parent) {};
  void updateAlert(const Alert &a, const QColor &color);

protected:
  void paintEvent(QPaintEvent*) override;

private:
  QColor bg;
  Alert alert = {};
};

// container window for the NVG UI
class NvgWindow : public CameraViewWidget {
  Q_OBJECT

public:
  explicit NvgWindow(VisionStreamType type, QWidget* parent = 0);
  OnroadHud *hud;
  
protected:
  void paintGL() override;
  void initializeGL() override;
  void showEvent(QShowEvent *event) override;
  void updateFrameMat(int w, int h) override;
  void drawLaneLines(QPainter &painter, const UIScene &scene);
  void drawLead(QPainter &painter, const UIScene &scene,
                const cereal::ModelDataV2::LeadDataV3::Reader &lead_data, 
                const cereal::RadarState::LeadData::Reader &radar_lead_data, 
                const QPointF &vd, bool cluspeedms, bool is_radar);
  inline QColor redColor(int alpha = 200) { return QColor(201, 34, 49, alpha); }
  inline QColor blackColor(int alpha = 200) { return QColor(0, 0, 0, alpha); }
  inline QColor greenColor(int alpha = 200) { return QColor(49, 201, 34, alpha); }
  inline QColor graceBlueColor(int alpha = 200) { return QColor (34, 49, 201, alpha); }
  inline QColor skyBlueColor(int alpha = 200) { return QColor (135, 206, 230, alpha); }
  inline QColor tomatoColor(int alpha = 250) { return QColor (255, 99, 71, alpha); }
  
  double prev_draw_t = 0;
  FirstOrderFilter fps_filter;
  
  // neokii
  QPixmap ic_brake;
  QPixmap ic_autohold_warning;
  QPixmap ic_autohold_active;
  QPixmap ic_nda;
  QPixmap ic_hda;
  QPixmap ic_tire_pressure;
  QPixmap ic_turn_signal_l;
  QPixmap ic_turn_signal_r;
  QPixmap ic_satellite;
  QPixmap ic_bsd_l;
  QPixmap ic_bsd_r;
  QPixmap ic_lcr;
  
  QMap<QString, QPixmap> ic_oil_com;
  
  void drawText(QPainter &p, int x, int y, const QString &text, int alpha = 255);
  void drawTextWithColor(QPainter &p, int x, int y, const QString &text, QColor& color);
  void drawText2(QPainter &p, int x, int y, int flags, const QString &text, const QColor& color);
  void drawMaxSpeed(QPainter &p);
  void drawSpeed(QPainter &p);
  void drawBottomIcons(QPainter &p);
  void drawSpeedLimit(QPainter &p);
  void drawTurnSignals(QPainter &p);
  void drawGpsStatus(QPainter &p);
  void drawDebugText(QPainter &p);
  void drawCgear(QPainter &p);//기어
  void drawBsd(QPainter &p);//bsd
  void drawTpms(QPainter &p);
  void drawBrake(QPainter &p);
  void drawLcr(QPainter &p);
  void drawCommunity(QPainter &p);
  void drawRestArea(QPainter &p);
  void drawSteer(QPainter &p);
  void drawEngRpm(QPainter &p);
  
  const int radius = 192;
  const int img_size = (radius / 2) * 1.5;
  
private:
  QPixmap get_icon_iol_com(const char* key);
  void drawRestAreaItem(QPainter &p, int yPos, capnp::Text::Reader image, capnp::Text::Reader title,
                        capnp::Text::Reader oilPrice, capnp::Text::Reader distance, bool lastItem);  
  
signals:
  void resizeSignal(int w);  
};

// container for all onroad widgets
class OnroadWindow : public QWidget {
  Q_OBJECT

public:
  OnroadWindow(QWidget* parent = 0);
  bool isMapVisible() const { return map && map->isVisible(); }

protected:
  void mousePressEvent(QMouseEvent* e) override;
  void mouseReleaseEvent(QMouseEvent* e) override;
  
  void paintEvent(QPaintEvent *event) override;

private:
  OnroadHud *hud;
  OnroadAlerts *alerts;
  NvgWindow *nvg;
  ButtonsWindow *buttons;
  QColor bg = bg_colors[STATUS_DISENGAGED];
  QWidget *map = nullptr;
  QHBoxLayout* split;

  // neokii
#ifdef QCOM2
private:
  ScreenRecoder* recorder;
  std::shared_ptr<QTimer> record_timer;
  QPoint startPos;
#endif

private slots:
  void offroadTransition(bool offroad);
  void updateState(const UIState &s);
};

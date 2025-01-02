#ifndef APP_MAINWINDOW_HPP_
#define APP_MAINWINDOW_HPP_

#include <QtWidgets/QMainWindow>

#include "ILocationManager.hpp"
#include "IpLocationData.hpp"

class ILocationManager;
class QPushButton;
class QMenuBar;
class QLineEdit;
class QGridLayout;
class QLabel;
class QCheckBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(std::unique_ptr<ILocationManager> locationManager);
    virtual ~MainWindow() = default;

private slots:
    void handleAddLocationButton();

private:
    static constexpr int GRID_WIDTH = 3;
    static constexpr int RESULTS_TO_DISPLAY = 5;
    static constexpr int LINES_PER_SINGLE_RESULT = 2;
    static constexpr int RESULT_LABELS_NUMBER = RESULTS_TO_DISPLAY * LINES_PER_SINGLE_RESULT;

    std::unique_ptr<ILocationManager> mLocationManager;

    QLineEdit* mGetLocationLineEdit;
    QPushButton* mGetLocationPushButton;
    QCheckBox* mGetLocationWithForceCheckBox;
    QCheckBox* mGetLocationOnlyDbCheckBox;

    std::array<QLabel*, RESULT_LABELS_NUMBER> mLocationResults;

    QMenuBar* createMenuBar();
    int addLocationAddToGridLayout(QGridLayout* layout, int level);
    int addConfigurationToGridLayout(QGridLayout* layout, int level);
    int addStatusToGridLayout(QGridLayout* layout, int level);

    void handleNewLocation(IpLocationData ipLocationData);
    void shiftResults();
};
#endif // APP_MAINWINDOW_HPP_
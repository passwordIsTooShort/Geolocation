#ifndef APP_MAINWINDOW_HPP_
#define APP_MAINWINDOW_HPP_

#include <QtWidgets/QMainWindow>
#include <QtCore/QStringBuilder>

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

    struct LocationAddConfig
    {
        bool forceUpdate;
        bool searchOnlyDb;
    };

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

    void printNewLocationError(QString firstLineError, QString secondLineError);
    void handleNewLocation(IpLocationData ipLocationData);
    void shiftResults();

    template <typename T>
    void handleGetLocation(T ipOrUrl, LocationAddConfig config)
    {
        if (config.forceUpdate)
        {
            mLocationManager->updateLocation(ipOrUrl);
            return;
        }

        const auto resultsList = mLocationManager->getLocations(ipOrUrl);
        if (resultsList.empty())
        {
            if (config.searchOnlyDb)
            {
                QString firstLineError = "Failed to get location for: " %
                                      QString::fromStdString(ipOrUrl.toString());
                QString secondLineError = "Location not available in DB, and \"only DB\" is checked in configuration";
                printNewLocationError(firstLineError, secondLineError);
            }
            else
            {
                mLocationManager->addLocation(ipOrUrl);
            }
        }
        else
        {
            for (auto&& singleLocatin : resultsList)
            {
                handleNewLocation(singleLocatin);
            }
        }
    }
};
#endif // APP_MAINWINDOW_HPP_
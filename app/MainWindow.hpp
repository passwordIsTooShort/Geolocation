#ifndef APP_MAINWINDOW_HPP_
#define APP_MAINWINDOW_HPP_

#include <QtWidgets/QMainWindow>
#include <QtCore/QStringBuilder>

#include "IpLocationData.hpp"
#include "AppConfigurationData.hpp"

class ILocationManager;
class QPushButton;
class QMenuBar;
class QLineEdit;
class QGridLayout;
class QLabel;
class QCheckBox;

struct LocationAddConfig
{
    bool forceUpdate;
    bool searchOnlyDb;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow() = default;

    void showWarning(QString warning);

signals:
    void requestToGetLocation(QString ipOrUrl, LocationAddConfig config);
    void saveConfiguration(AppConfigurationData appConfig);

private slots:
    void handleGetLocationButton();
    void handleSaveConfigurationButton();

public slots:
    void onNewLocation(IpLocationData ipLocationData);

private:
    static constexpr int GRID_WIDTH = 3;
    static constexpr int RESULTS_TO_DISPLAY = 5;
    static constexpr int LINES_PER_SINGLE_RESULT = 2;
    static constexpr int RESULT_LABELS_NUMBER = RESULTS_TO_DISPLAY * LINES_PER_SINGLE_RESULT;

    static constexpr char const* FILE_CONFIGURATION_NAME = "Configuration";
    static constexpr char const* FILE_LOAD_FROM_FILE_NAME = "Load from file";
    static constexpr char const* FILE_EXIT_NAME = "Exit";
    static constexpr char const* DIAG_VERIFY_DB_NAME = "Verify connection to DB";
    static constexpr char const* DIAG_GET_FAIL_LIST_NAME = "Get failed requests list";
    static constexpr char const* DIAG_REQ_FROM_STARTUP_LIST_NAME = "Get list of requests from startup";

    // Main window labels
    QLineEdit* mGetLocationLineEdit;
    QPushButton* mGetLocationPushButton;
    QCheckBox* mGetLocationWithForceCheckBox;
    QCheckBox* mGetLocationOnlyDbCheckBox;

    // Configuration labels
    QLineEdit* mApiKeyLineEdit;
    QLineEdit* mDatabaseLocationLineEdit;
    QLineEdit* mTableNameLineEdit;
    QPushButton* mSaveConfigurationPushButton;

    std::array<QLabel*, RESULT_LABELS_NUMBER> mLocationResults;

    QMenuBar* createMenuBar();
    int addLocationAddToGridLayout(QGridLayout* layout, int level);
    int addConfigurationToGridLayout(QGridLayout* layout, int level);
    int addStatusToGridLayout(QGridLayout* layout, int level);

    void handleMenu(QAction* action);
    void openConfigurationMenu();

    void shiftResults();
};
#endif // APP_MAINWINDOW_HPP_
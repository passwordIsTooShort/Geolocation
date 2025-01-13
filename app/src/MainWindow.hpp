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
class QFrame;
class QMessageBox;

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
    virtual ~MainWindow();

    enum class DialogLevel
    {
        WARNING,
        INFO
    };

    void showDialog(QString text, DialogLevel dialogLevel = DialogLevel::WARNING);

signals:
    void requestToGetLocation(QString ipOrUrl, LocationAddConfig config);
    void requestToRemoveLocation(QString ipOrUrl);
    void saveConfiguration(AppConfigurationData appConfig);

private slots:
    void handleGetLocationButton();
    void handleRemoveLocationButton();
    void handleSaveConfigurationButton();
    void handleLoadMultipleButton();

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

    QMenu * mFileMenu;
    QMenu * mDiagnosticsMenu;
    QGridLayout* mMainGridLayout;

    // Main window labels
    QWidget* mMainWidget;
    QFrame* mLineSeparator;
    QLabel* mGetLocationLabel;
    QLineEdit* mGetLocationLineEdit;
    QPushButton* mGetLocationPushButton;
    QPushButton* mRemoveLocationPushButton;
    QCheckBox* mGetLocationWithForceCheckBox;
    QCheckBox* mGetLocationOnlyDbCheckBox;

    // Configuration labels
    QWidget* mConfigurationWindow;
    QGridLayout* mConfigurationWindowLayout;
    QLineEdit* mApiKeyLineEdit;
    QLineEdit* mDatabaseLocationLineEdit;
    QLineEdit* mTableNameLineEdit;
    QPushButton* mSaveConfigurationPushButton;
    QLabel* mApiLabel;
    QLabel* mDatabaseLocationLabel;
    QLabel* mTableNameLabel;

    // Load multiple ips labels
    QWidget* mLoadMultipleWindow;
    QGridLayout* mLoadMultipleWindowLayout;
    QLabel* mLoadMultipleLabel;
    QLineEdit* mLoadMultipleFileLineEdit;
    QPushButton* mLoadMultiplePushButton;

    // Message box
    QMessageBox* mMsgbox;
    std::array<QLabel*, RESULT_LABELS_NUMBER> mLocationResults;

    void createMenuBar();
    int addLocationAddToGridLayout(int level);
    int addConfigurationToGridLayout(int level);
    int addStatusToGridLayout(int level);

    void handleMenu(QAction* action);
    void openConfigurationMenu();
    void openLoadMultipleMenu();

    void shiftResults();
};
#endif // APP_MAINWINDOW_HPP_
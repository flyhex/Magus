/****************************************************************************
**
** Copyright (C) 2016
**
** This file is part of the Magus toolkit
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include <QDir>
#include <QtGlobal>
#include <QSettings>
#include <QMessagebox>
#include <QCursor>
#include "constants.h"
#include "qtbuilder.h"
#include "qtbuildermenu.h"
#include "qtbuilderaction.h"
#include "qtbuildertoolbar.h"
#include "qtbuildertab.h"
#include "qtbuilderogre.h"
#include "qtbuilderogre19.h"
#include "qtbuilderogre20.h"
#include "qtbuilderogre21.h"
#include "mainwindow.h"
#include "utils.h"

//****************************************************************************/
QtBuilder::QtBuilder()
{
    // Get the config settings. Although this is also done in the mainwindow,
    // it is done here also, because of the builde-specific settings. Builder-
    // specific settings should not be read in the mainwindow.
    // Load the global settings
    QSettings globalSettings (GLOBAL_CONFIG_FILE,  QSettings::IniFormat);
    mIconDir = globalSettings.value(CONFIG_KEY_ICON_DIR).toString();;
    mQtDir = globalSettings.value(CONFIG_KEY_QT_DIR).toString();
    mQtHeader = mQtDir + globalSettings.value(CONFIG_KEY_QTH_DIR).toString();
    mQtSrc = mQtDir + globalSettings.value(CONFIG_KEY_QTSRC_DIR).toString();
    mOutputDir = globalSettings.value(CONFIG_KEY_OUTPUT_DIR).toString();
    mOutputBinDir = globalSettings.value(CONFIG_KEY_OUTPUT_BIN_DIR).toString();
    mOutputIconDir = globalSettings.value(CONFIG_KEY_OUTPUT_ICON_DIR).toString();
    mOutputHeaderDir = globalSettings.value(CONFIG_KEY_OUTPUT_HEADER_DIR).toString();
    mOutputSrcDir = globalSettings.value(CONFIG_KEY_OUTPUT_SRC_DIR).toString();
    mOgreDir = "";
    mOutputOgreDir = "";
    bool ogreRootUseEnv = globalSettings.value(CONFIG_KEY_OGRE_ROOT_USE_ENV).toBool();
    QString ogreRootEnv = globalSettings.value(CONFIG_KEY_OGRE_ROOT_ENV).toString();
    QString ogreRoot = globalSettings.value(CONFIG_KEY_OGRE_ROOT).toString();
    QString ogreBuildDir = globalSettings.value(CONFIG_KEY_OGRE_BUILD_DIR).toString();
    QString ogreVersion = globalSettings.value(CONFIG_KEY_OGRE_VERSION).toString();
    mQtOgreBuilder = 0;
    setOgre(ogreRootUseEnv, ogreRootEnv, ogreRoot, ogreBuildDir, ogreVersion);
    mCurrentProject = QString("");
    mCurrentProjectSlash = QString("");
}

//****************************************************************************/
QtBuilder::~QtBuilder()
{
    if (mQtOgreBuilder)
        delete mQtOgreBuilder;
}

//****************************************************************************/
void QtBuilder::setOutputDir (const QString& outputDir)
{
    mOutputDir = outputDir;
}

//****************************************************************************/
void QtBuilder::setOgre (bool ogreRootUseEnv,
                         const QString& ogreRootEnv,
                         const QString& ogreRoot,
                         const QString& ogreBuildDir,
                         const QString& ogreVersion)
{
    if (ogreRootUseEnv && !ogreRootEnv.isEmpty())
    {
        // Use the environment variable
        QByteArray value = qgetenv(ogreRootEnv.toUtf8().constData());
        mOgreRoot = QString (value.constData()).toUtf8();
    }
        else
        {
        if (ogreRoot.isEmpty())
        {
            // Use the value from the config file (if the environment variable was empty, use the value from the config file instead)
            QSettings globalSettings (GLOBAL_CONFIG_FILE,  QSettings::IniFormat);
            mOgreRoot = globalSettings.value(CONFIG_KEY_OGRE_ROOT).toString();
        }
        else
        {
            // Use the second argument, since the environment variable was empty
            mOgreRoot = ogreRoot;
        }
    }
    mOgreRoot.remove(QRegExp(QString::fromUtf8("\""))); // Strip from double quotes
    mOgreRoot.replace(QChar('\\'), QChar('/')); // Change backslash into forward slash
    mOgreRoot = Utils::stripTrailingCharacter(mOgreRoot, '/'); // remove trailing slash (if available)

    mOgreBuildDir = ogreBuildDir;
    mOgreBuildDir.remove(QRegExp(QString::fromUtf8("\""))); // Strip from double quotes
    mOgreBuildDir = Utils::stripLeadingCharacter(mOgreBuildDir, '\\'); // remove leading backslash
    mOgreBuildDir = Utils::stripLeadingCharacter(mOgreBuildDir, '/'); // remove leading slash
    mOgreBuildDir = Utils::stripTrailingCharacter(mOgreBuildDir, '\\'); // remove trailing backslash
    mOgreBuildDir = Utils::stripTrailingCharacter(mOgreBuildDir, '/'); // remove trailing slash

    if (mQtOgreBuilder)
        delete mQtOgreBuilder;

    if (ogreVersion == "1.9")
    {
        mQtOgreBuilder = new QtOgre19Builder();
        mOgreDir = OGRE_SRC_DIR;
        mOutputOgreDir = OGRE_OUTPUT_DIR;
    }
    else
    {
        if (ogreVersion == "2.0")
        {
            mQtOgreBuilder = new QtOgre20Builder();
            mOgreDir = OGRE2_SRC_DIR;
            mOutputOgreDir = OGRE2_OUTPUT_DIR;
        }
        else
        {
            if (ogreVersion == "2.1")
            {
                mQtOgreBuilder = new QtOgre21Builder();
                mOgreDir = OGRE3_SRC_DIR;
                mOutputOgreDir = OGRE3_OUTPUT_DIR;
            }
            else
            {
                mQtOgreBuilder = new QtOgre21Builder(); // Just get the latest and see what happens
                mOgreDir = OGRE3_SRC_DIR;
                mOutputOgreDir = OGRE3_OUTPUT_DIR;
            }
        }
    }
}

//****************************************************************************/
void QtBuilder::build(ApplicationTemplate* applicationTemplate)
{
    if (!applicationTemplate)
        return;

    // Determine current project
    mCurrentProject = applicationTemplate->mProject;
    mCurrentProject = Utils::stripSpecialCharacters(mCurrentProject);
    if (mCurrentProject.isEmpty())
        return;

    mCurrentProjectSlash = mCurrentProject + QString("/");
    QtMenuBuilder qtMenuBuilder; // Delegate building menu related code to the QtMenuBuilder
    QtActionBuilder qtActionBuilder; // Delegate building action related code to the QtActionBuilder
    QtToolbarBuilder qtToolbarBuilder; // Delegate building toolbar related code to the QtToolbarBuilder
    QtTabBuilder qtTabBuilder; // Delegate building tab related code to the QtTabBuilder
    QDir dirUtil;
    QFile fileUtil;
    QString project; // Content of <project>.pro
    QString mainTitle; // Title on top of the mainwindow
    QString mainWindowSrc; // Content of mainwindow.ctp
    QString mainWindowHeader; // Content of mainwindow.htp
    QString dockWidgetHeaderBase; // Content of dockwidget.htp
    QString dockWidgetSrcBase; // Content of dockwidget.ctp
    QString dockWidgetHeader; // Content of <window>_dockwidget.h
    QString dockWidgetSrc; // Content of <window>_dockwidget.cpp
    QString dockWidgetHeaderFilename; // Name of the <window>_dockwidget.h file
    QString dockWidgetSrcFilename; // Name of the <window>_dockwidget.cpp file
    QString projectAdditionalHeader; // Used to add additional .h files to the project file
    QString projectAdditionalSrc; // Used to add additional .cpp files to the project file
    QString mainWindowAdditionalInclude; // Used to add additional #includes to mainwindow.h
    QString mainWindowPrivateSlots; // Used to add additional private slots to mainwindow.h
    QString mainWindowAdditionalPublic;  // Used to add additional public members to mainwindow.h
    QString mainWindowAdditionalPrivate; // Used to add additional private members to mainwindow.h
    QString mainWindowOgreConstructor; // Used for Ogre-specific code in the constructor of mainwindow.cpp
    QString mainWindowOgreDestructor; // Used for Ogre-specific code in the destructor of mainwindow.cpp
    QString mainWindowOgreUpdate; // Used for Ogre-specific code in the update function of mainwindow.cpp
    QString mainWindowOgreInitialize; // If Ogre is used, the rendermanager may be initialized
    QString mainWindowCreateDockWidgets; // Used to fill the createDockWidgets function in mainwindow.cpp
    QString mainWindowCreateMenu; // Used to fill the createMenu function in mainwindow.cpp
    QString mainWindowCreateActions; // Used to fill the createActions function in mainwindow.cpp
    QString mainWindowCreateToolbars; // Used to fill the createToolbars function in mainwindow.cpp
    QString mainWindowDoActions; // Used to fill the doAction functions in mainwindow.cpp
    QString mainWindowCreateOgreWidget;  // Used to create the Ogre Widget in mainwindow.cpp
    QString dockWidgetSrcCreateConstructor; // Used to fill the constructor in <window>_dockwidget.cpp files
    QString dockWidgetSrcCreateConstructorEnd; // Used to fill the last part of the constructor in <window>_dockwidget.cpp files
    QString dockWidgetSrcCreateMenu; // Used to fill the createMenu function in <window>_dockwidget.cpp files
    QString dockWidgetSrcCreateActions; // Used to fill the createActions function in <window>_dockwidget.cpp files
    QString dockWidgetSrcDoActions; // Used to fill the doAction functions in <window>_dockwidget.cpp files
    QString dockWidgetCreateToolbars; // Used to fill the createToolbars function in <window>_dockwidget.cpp files
    QString dockWidgetCreateOgreWidget; // Used to create the Ogre Widget in <window>_dockwidget.cpp files
    QString dockWidgetHeaderAdditionalInclude; // Used to add additional #includes to <window>_dockwidget.h files
    QString dockWidgetHeaderAdditionalPrivate; // Used to add additional private members to <window>_dockwidget.h files
    QString dockWidgetHeaderPrivateSlots; // Used to add additional private slots to <window>_dockwidget.h files
    QString projectModules = QString("QT += widgets"); // Default modules used
    QString projectModuleOpenGL = QString(" opengl"); // OpenGL module (preceeded by a space)
    QString projectModuleMedia = QString(" multimedia"); // Multimedia module (preceeded by a space)
    QString projectOgreRoot;// Used to set the root directory in the .pro file
    QString ogreInclude; // Used for additional includes in the .pro file
    QString ogreLib; // Used for additional libs in the .pro file
    QString privateAttribute;
    //QString menuItemName;
    bool transformationWidget = false;
    bool useAssetWidget = false;
    bool ogreControlWidget = false; // Only used to indicate that there is at least one window with an Ogre widget
    bool ogreAssetWidgets = false; // Use Ogre Asset widgets
    bool useNodeEditor = false; // Use Node editor
    bool useTools = false; // Use Tool items

    // --------------------------------------------------------------------------------------------------------------------
    // Step 1: Delete old output files and directories
    Utils::removeDirRecursively(mOutputDir + mCurrentProjectSlash);

    // --------------------------------------------------------------------------------------------------------------------
    // Step 2: Create new output directories
    mFullOutputHeaderDir = mOutputDir + mCurrentProjectSlash + mOutputHeaderDir;
    mFullOutputBinDir = mOutputDir + mCurrentProjectSlash + mOutputBinDir;
    mFullOutputIconDir = mOutputDir + mCurrentProjectSlash + mOutputIconDir;
    mFullOutputSrcDir = mOutputDir + mCurrentProjectSlash + mOutputSrcDir;
    dirUtil.mkpath(mFullOutputBinDir);
    dirUtil.mkpath(mFullOutputIconDir);
    dirUtil.mkpath(mFullOutputHeaderDir);
    dirUtil.mkpath(mFullOutputSrcDir);
    dirUtil.mkpath(mFullOutputBinDir + PATH_QT_DLL_WINDOWS);

    // --------------------------------------------------------------------------------------------------------------------
    // Step 3: Copy the base files (that do not need to be changed)
    fileUtil.copy(mQtHeader + FILE_MAGUS_CORE_H, mFullOutputHeaderDir + FILE_MAGUS_CORE_H);
    fileUtil.copy(mQtSrc + FILE_MAIN_CTP, mFullOutputSrcDir + FILE_MAIN_CPP);
    fileUtil.copy(mQtDir + STYLE_DARK, mFullOutputBinDir + STYLE_DARK);

//    fileUtil.copy(FILE_QT_DLL_CORE, mFullOutputBinDir + FILE_QT_DLL_CORE);
//    fileUtil.copy(FILE_QT_DLL_WIDGETS, mFullOutputBinDir + FILE_QT_DLL_WIDGETS);
//    fileUtil.copy(FILE_QT_DLL_GUI, mFullOutputBinDir + FILE_QT_DLL_GUI);
//    fileUtil.copy(FILE_QT_DLL_ICUDT, mFullOutputBinDir + FILE_QT_DLL_ICUDT);
//    fileUtil.copy(FILE_QT_DLL_ICUIN, mFullOutputBinDir + FILE_QT_DLL_ICUIN);
//    fileUtil.copy(FILE_QT_DLL_ICUUC, mFullOutputBinDir + FILE_QT_DLL_ICUUC);
//    fileUtil.copy(PATH_QT_DLL_WINDOWS + FILE_QT_DLL_WINDOWS, mFullOutputBinDir + PATH_QT_DLL_WINDOWS + FILE_QT_DLL_WINDOWS);

    // --------------------------------------------------------------------------------------------------------------------
    // Step 4: Mark that the asset/node/tool widgets are used
    if (applicationTemplate->mUseAssetWidget)
    {
        useAssetWidget = true; // Generic assets
    }
    if (applicationTemplate->mUseOgreAssetWidgets)
    {
        ogreAssetWidgets = true; // Ogre assets
    }
    if (applicationTemplate->mUseNodeEditor)
    {
        useNodeEditor = true; // Node editor
    }
    if (applicationTemplate->mUseTools)
    {
        useTools = true; // Tool items
        projectModules += projectModuleOpenGL; // Add OpenGL module
        projectModules += projectModuleMedia; // Add Multimedia module
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 5: Get the ctp, htp, ptp files as string
    mainWindowSrc = Utils::getFileAsString(mQtSrc + FILE_MAINWINDOW_CTP);
    mainWindowHeader = Utils::getFileAsString(mQtHeader + FILE_MAINWINDOW_HTP);
    project = Utils::getFileAsString(mQtDir + FILE_PROJECT_PTP);
    dockWidgetHeaderBase = Utils::getFileAsString(mQtHeader + FILE_DOCKWIDGET_HTP);
    dockWidgetSrcBase = Utils::getFileAsString(mQtSrc + FILE_DOCKWIDGET_CTP);

    // --------------------------------------------------------------------------------------------------------------------
    // Step 6: Loop through the windows, these become the DockWidgets, except for the window with order == 0 (which is mainwindow)
    QVectorIterator<WindowProperties*> list(applicationTemplate->mWindowPropertiesList);
    WindowProperties* windowProperties = 0;
    QString strippedTitle; // If the title contains spaces or special characters, they are stripped; the strippedTitle is used in a classname
    while (list.hasNext())
    {
        windowProperties = list.next();
        if (windowProperties->mUseTransformationWidget)
        {
            transformationWidget = true;
        }
        if (windowProperties->mUseOgreControl)
        {
            ogreControlWidget = true;
        }

        strippedTitle = Utils::stripSpecialCharacters(windowProperties->mTitle);
        if (windowProperties->mOrder == 0)
        {
            mainTitle = windowProperties->mTitle;

            /* --------------------------------------------------------------------------------------------------------------------
               - mainwindow.h -
               --------------------------------------------------------------------------------------------------------------------*/

            // Create the menu in the mainwindow.h
            mainWindowAdditionalInclude = qtMenuBuilder.createInclude(mainWindowAdditionalInclude);
            mainWindowAdditionalPrivate = qtMenuBuilder.createPrivateMembers(windowProperties->mWindowMenuProperties,
                                                                                mainWindowAdditionalPrivate);

            // Create the menu in the mainwindow.cpp
            mainWindowCreateMenu = qtMenuBuilder.createMenuMainWindow(windowProperties->mWindowMenuProperties, mainWindowCreateMenu);

            // Create the actions in the mainwindow.h
            mainWindowAdditionalInclude = qtActionBuilder.createInclude(mainWindowAdditionalInclude);
            mainWindowAdditionalPrivate = qtActionBuilder.createPrivateMembers(windowProperties,
                                                                               windowProperties->mWindowMenuProperties,
                                                                               mainWindowAdditionalPrivate);
            if (windowProperties->mUseOgreControl)
            {
                // Add the QtOgreWidget member
                mainWindowAdditionalPrivate = mQtOgreBuilder->createPrivateMemberQtOgreWidget(mainWindowAdditionalPrivate);
            }
            mainWindowPrivateSlots = qtActionBuilder.createPrivateSlots(windowProperties,
                                                                        windowProperties->mWindowMenuProperties,
                                                                        mainWindowPrivateSlots);

            // Create the menu in the mainwindow.cpp
            mainWindowCreateActions = qtActionBuilder.createActions(windowProperties,
                                                                    windowProperties->mWindowMenuProperties,
                                                                    mainWindowCreateActions,
                                                                    mIconDir,
                                                                    mFullOutputIconDir);
            mainWindowDoActions = qtActionBuilder.createDoActions(applicationTemplate,
                                                                  windowProperties,
                                                                  windowProperties->mWindowMenuProperties,
                                                                  mainWindowDoActions);

            // Create the toolbars in the mainwindow.h
            mainWindowAdditionalInclude = qtToolbarBuilder.createInclude(windowProperties, mainWindowAdditionalInclude);
            mainWindowAdditionalPrivate = qtToolbarBuilder.createPrivateMembers(windowProperties, mainWindowAdditionalPrivate);

            /* --------------------------------------------------------------------------------------------------------------------
               - mainwindow.cpp -
               --------------------------------------------------------------------------------------------------------------------*/

            // Create the toolbars in the mainwindow.cpp
            mainWindowCreateToolbars = qtToolbarBuilder.createToolbars(windowProperties, mainWindowCreateToolbars);

            if (windowProperties->mUseOgreControl)
            {
                // Create the Ogre widget in the mainwindow.cpp
                mainWindowCreateOgreWidget = mQtOgreBuilder->createQtOgreWidgetForMainWindow(windowProperties, mainWindowCreateOgreWidget);
            }
        }
        else
        {
            /* --------------------------------------------------------------------------------------------------------------------
               - <window>_dockwidget.h -
               --------------------------------------------------------------------------------------------------------------------*/
            dockWidgetHeader = dockWidgetHeaderBase;
            dockWidgetHeaderAdditionalInclude = QString("");
            dockWidgetHeaderAdditionalPrivate = QString("");
            dockWidgetHeaderPrivateSlots = QString("");

            // Create generic private members in the dockwidget.h
            dockWidgetHeaderAdditionalPrivate = createGenericPrivateMembers(windowProperties, dockWidgetHeaderAdditionalPrivate);

            // Create the menu entries in the dockwidget.h
            dockWidgetHeaderAdditionalInclude = qtMenuBuilder.createInclude(dockWidgetHeaderAdditionalInclude);
            dockWidgetHeaderAdditionalPrivate = qtMenuBuilder.createPrivateMembers(windowProperties->mWindowMenuProperties,
                                                                                    dockWidgetHeaderAdditionalPrivate);

            // Create the action entries in the dockwidget.h
            dockWidgetHeaderAdditionalInclude = qtActionBuilder.createInclude(dockWidgetHeaderAdditionalInclude);
            dockWidgetHeaderAdditionalPrivate = qtActionBuilder.createPrivateMembers(windowProperties,
                                                                                     windowProperties->mWindowMenuProperties,
                                                                                     dockWidgetHeaderAdditionalPrivate);
            dockWidgetHeaderPrivateSlots = qtActionBuilder.createPrivateSlots(windowProperties,
                                                                              windowProperties->mWindowMenuProperties,
                                                                              dockWidgetHeaderPrivateSlots);

            // Create the toolbar entries in the dockwidget.h
            dockWidgetHeaderAdditionalInclude = qtToolbarBuilder.createInclude(windowProperties, dockWidgetHeaderAdditionalInclude);
            dockWidgetHeaderAdditionalPrivate = qtToolbarBuilder.createPrivateMembers(windowProperties, dockWidgetHeaderAdditionalPrivate);

            // Create the tab entries in the dockwidget.h
            dockWidgetHeaderAdditionalInclude = qtTabBuilder.createInclude(windowProperties, dockWidgetHeaderAdditionalInclude);
            dockWidgetHeaderAdditionalPrivate = qtTabBuilder.createPrivateMembers(windowProperties, dockWidgetHeaderAdditionalPrivate);
            dockWidgetHeaderPrivateSlots = qtTabBuilder.createPrivateSlots(windowProperties, dockWidgetHeaderPrivateSlots);

            // Create the Ogre entries in the dockwidget.h
            if (windowProperties->mUseOgreControl)
            {
                dockWidgetHeaderAdditionalInclude = mQtOgreBuilder->createInclude(dockWidgetHeaderAdditionalInclude);
                dockWidgetHeaderAdditionalPrivate = mQtOgreBuilder->createPrivateMemberQtOgreWidget(dockWidgetHeaderAdditionalPrivate);
            }

            // Replace the properties in the dockwidget.h file
            dockWidgetHeader.replace(DOCKWIDGET_TITLE, strippedTitle);
            dockWidgetHeader.replace(DOCKWIDGET_INCLUDES, dockWidgetHeaderAdditionalInclude);
            dockWidgetHeader.replace(DOCKWIDGET_PRIVATE, dockWidgetHeaderAdditionalPrivate);
            dockWidgetHeader.replace(DOCKWIDGET_PRIVATE_SLOTS, dockWidgetHeaderPrivateSlots);
            dockWidgetHeaderFilename = strippedTitle + QString("_dockwidget.h");
            dockWidgetHeaderFilename = dockWidgetHeaderFilename.toLower();
            Utils::writeStringtoFile(dockWidgetHeader, mFullOutputHeaderDir + dockWidgetHeaderFilename);

            /* --------------------------------------------------------------------------------------------------------------------
               - <window>_dockwidget.cpp -
               --------------------------------------------------------------------------------------------------------------------*/
            dockWidgetSrc = dockWidgetSrcBase;
            dockWidgetSrcCreateConstructor = QString("");
            dockWidgetSrcCreateConstructorEnd = QString("");
            dockWidgetSrcCreateMenu = QString("");
            dockWidgetSrcCreateActions = QString("");
            dockWidgetSrcDoActions = QString("");
            dockWidgetCreateToolbars = QString("");
            dockWidgetCreateOgreWidget = QString("");

            dockWidgetSrc.replace(DOCKWIDGET_TITLE, strippedTitle);

            // Create entries in the constructor of the dockwidget.cpp
            dockWidgetSrcCreateConstructor = createConstructor(dockWidgetSrcCreateConstructor);

            // Create menu entries in the dockwidget.cpp
            dockWidgetSrcCreateMenu = qtMenuBuilder.createMenuDockWidget(windowProperties->mWindowMenuProperties, dockWidgetSrcCreateMenu);

            // Create action entries in the dockwidget.cpp
            dockWidgetSrcCreateActions = qtActionBuilder.createActions(windowProperties,
                                                                       windowProperties->mWindowMenuProperties,
                                                                       dockWidgetSrcCreateActions,
                                                                       mIconDir,
                                                                       mFullOutputIconDir);
            dockWidgetSrcDoActions = qtActionBuilder.createDoActions(applicationTemplate,
                                                                    windowProperties,
                                                                    windowProperties->mWindowMenuProperties,
                                                                    dockWidgetSrcDoActions);


            // Create toolbar entries in the dockwidget.cpp
            dockWidgetCreateToolbars = qtToolbarBuilder.createToolbars(windowProperties, dockWidgetCreateToolbars);

            // Create tabs in the constructor of the dockwidget.cpp
            dockWidgetSrcCreateConstructor = qtTabBuilder.createTabs(windowProperties,
                                                                     dockWidgetSrcCreateConstructor,
                                                                     mIconDir,
                                                                     mFullOutputIconDir);
            dockWidgetSrcCreateConstructorEnd = qtTabBuilder.createConnect(windowProperties, dockWidgetSrcCreateConstructorEnd);
            dockWidgetSrcDoActions = qtTabBuilder.createTabSelected(windowProperties, dockWidgetSrcDoActions);

            // Create the Ogre entries in the dockwidget.cpp
            if (windowProperties->mUseOgreControl)
            {
                if (windowProperties->mOgreTarget == 0)
                {
                    // Set it in the window
                    dockWidgetCreateOgreWidget = mQtOgreBuilder->createQtOgreWidgetForDockWindow(windowProperties, dockWidgetCreateOgreWidget);
                }
                else
                {
                    // Set it in the tab
                    int index = windowProperties->mOgreTarget;
                    --index;
                    dockWidgetCreateOgreWidget = mQtOgreBuilder->createQtOgreWidgetForTab(windowProperties, index, dockWidgetCreateOgreWidget);
                }
            }

            // Replace the properties in the dockwidget.cpp file
            dockWidgetSrc.replace(DOCKWIDGET_CONSTRUCTOR, dockWidgetSrcCreateConstructor);
            dockWidgetSrc.replace(DOCKWIDGET_CONSTRUCTOR_END, dockWidgetSrcCreateConstructorEnd);
            dockWidgetSrc.replace(DOCKWIDGET_CREATE_MENUS, dockWidgetSrcCreateMenu);
            dockWidgetSrc.replace(DOCKWIDGET_CREATE_ACTIONS, dockWidgetSrcCreateActions);
            dockWidgetSrc.replace(DOCKWIDGET_CREATE_TOOLBARS, dockWidgetCreateToolbars);
            dockWidgetSrc.replace(DOCKWIDGET_DO_ACTIONS, dockWidgetSrcDoActions);
            dockWidgetSrc.replace(DOCKWIDGET_OGRE_CREATE_WIDGET, dockWidgetCreateOgreWidget);
            dockWidgetSrcFilename = strippedTitle + QString("_dockwidget.cpp");
            dockWidgetSrcFilename = dockWidgetSrcFilename.toLower();
            Utils::writeStringtoFile(dockWidgetSrc, mFullOutputSrcDir + dockWidgetSrcFilename);

            // ***************************** Create additional dockwidget entries to the mainwindow.h file *****************************
            // Add #include entries
            mainWindowAdditionalInclude = mainWindowAdditionalInclude +
                    QString("#include \"") +
                    dockWidgetHeaderFilename +
                    QString("\"") +
                    QString("\n");

            // Add private members
            privateAttribute = Utils::makePrivateAttribute(windowProperties->mTitle) + QString("DockWidget");
            mainWindowAdditionalPrivate = mainWindowAdditionalPrivate +
                    TAB +
                    TAB +
                    strippedTitle +
                    QString("DockWidget* ") +
                    privateAttribute +
                    QString(";") +
                    QString("\n");

            // ***************************** Create additional dockwidget entries to the mainwindow.src file *****************************
            // Elaborate the createDockWidgets() function
            mainWindowCreateDockWidgets = mainWindowCreateDockWidgets +
                    TAB +
                    privateAttribute +
                    QString(" = new ") +
                    strippedTitle + QString("DockWidget") +
                    QString("(\"") +
                    windowProperties->mTitle +
                    QString("\", this);") +
                    QString("\n");

            // Set the area of the dockwidget in the mainwindow
            mainWindowCreateDockWidgets = mainWindowCreateDockWidgets + qtActionBuilder.getDockWidgetToAreaString(windowProperties);

            /* --------------------------------------------------------------------------------------------------------------------
               - <window>_dockwidget.cpp -
               --------------------------------------------------------------------------------------------------------------------*/
            // Headers
            projectAdditionalHeader = projectAdditionalHeader +
                    TAB +
                    mOutputHeaderDir +
                    dockWidgetHeaderFilename +
                    QString(" \\ ") +
                    QString("\n");

            // Src
            projectAdditionalSrc = projectAdditionalSrc +
                    TAB +
                    mOutputSrcDir +
                    dockWidgetSrcFilename +
                    QString(" \\ ") +
                    QString("\n");
        }
    }


    // --------------------------------------------------------------------------------------------------------------------
    // Step 7: If the Transformationwidget is used, copy the h and cpp file and update the .pro file
    if (transformationWidget)
    {
        projectAdditionalHeader = createTransformationWidgetHeaderForPro(projectAdditionalHeader);
        projectAdditionalSrc = createTransformationWidgetSrcForPro(projectAdditionalSrc);
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 8: If an QtOgreWidget is used, copy the h, cpp and Ogre dll files and update the .pro, mainindow.h and
    // mainndow.cpp files
    if (ogreControlWidget)
    {
        // Create the dir
        mFullOutputOgreDir = mOutputDir + mCurrentProjectSlash + mOutputOgreDir;
        dirUtil.mkpath(mFullOutputOgreDir);

        // Update the .pro file
        projectAdditionalHeader = mQtOgreBuilder->createHeaderForPro(mOutputHeaderDir, projectAdditionalHeader);
        projectAdditionalSrc = mQtOgreBuilder->createSrcForPro(mOutputSrcDir, projectAdditionalSrc);
        projectOgreRoot = mQtOgreBuilder->createRootForPro(mOgreRoot, projectOgreRoot);
        ogreInclude = mQtOgreBuilder->createIncludeForPro(mOgreBuildDir, ogreInclude);
        ogreLib = mQtOgreBuilder->createLibForPro(mOgreBuildDir, ogreLib);

        // Update the mainwindow.h file
        // Add the Ogre Rendermanager #include
        mainWindowAdditionalInclude = mQtOgreBuilder->createInclude(mainWindowAdditionalInclude);

        // Define public/private member in the mainwindow.h file
        mainWindowAdditionalPublic = mQtOgreBuilder->createPublicMemberForMainWindow(mainWindowAdditionalPublic);
        mainWindowAdditionalPrivate = mQtOgreBuilder->createPrivateMemberForMainWindow(mainWindowAdditionalPrivate);

        // Update the mainwindow.cpp file
        mainWindowOgreConstructor = mQtOgreBuilder->createConstructorEntryForMainWindow(mainWindowOgreConstructor);
        mainWindowOgreDestructor = mQtOgreBuilder->createDestructorEntryForMainWindow(mainWindowOgreDestructor);
        mainWindowOgreUpdate = mQtOgreBuilder->createUpdateEntryForMainWindow(mainWindowOgreUpdate);
        mainWindowOgreInitialize = mQtOgreBuilder->createInitializeEntryForMainWindow(mainWindowOgreInitialize);

        // Copy the Ogre files
        mQtOgreBuilder->copyOgreFiles(mOgreRoot,
                                      mOgreBuildDir,
                                      mOgreDir,
                                      mQtHeader,
                                      mQtSrc,
                                      mFullOutputHeaderDir,
                                      mFullOutputSrcDir,
                                      mFullOutputBinDir,
                                      mFullOutputOgreDir);
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 9: If Ogre Asset widgets are used, copy the h, cpp and udate the .pro file. This is only possible in case
    // the ogre control itself is checked.
    if (ogreControlWidget && ogreAssetWidgets)
    {
        // Update the .pro file
        projectAdditionalHeader = mQtOgreBuilder->createOgreAssetHeaderForPro(mOutputHeaderDir, projectAdditionalHeader);
        projectAdditionalSrc = mQtOgreBuilder->createOgreAssetSrcForPro(mOutputSrcDir, projectAdditionalSrc);

        // Copy the Ogre asset files
        mQtOgreBuilder->copyOgreAssetFiles(mQtHeader, mQtSrc, mIconDir, mFullOutputHeaderDir, mFullOutputSrcDir, mFullOutputIconDir);

        useAssetWidget = true; // Generic assets
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 10: If the Asset widget is used, copy the h and cpp file and update the .pro file. Note, that useAssetWidget
    // was also set to 'true' in case ogreAssetWidgets is 'true', because the generic asset files are a prerequisite.
    if (useAssetWidget)
    {
        projectAdditionalHeader = createAssetHeaderForPro(projectAdditionalHeader); // This also copies files
        projectAdditionalSrc = createAssetSrcForPro(projectAdditionalSrc); // This also copies files
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 11: If the Node editor is used, copy the h and cpp file (and icons) and update the .pro file.
    if (useNodeEditor)
    {
        projectAdditionalHeader = createNodeHeaderForPro(projectAdditionalHeader); // This also copies files
        projectAdditionalSrc = createNodeSrcForPro(projectAdditionalSrc); // This also copies files
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 12: If the 'Tool' items are used, copy the h and cpp file (and icons) and update the .pro file.
    if (useTools)
    {
        projectAdditionalHeader = createToolHeaderForPro(projectAdditionalHeader); // This also copies files
        projectAdditionalSrc = createToolSrcForPro(projectAdditionalSrc); // This also copies files

        // Copy the DLL, only if GL stuff is used
        fileUtil.copy(FILE_QT_DLL_OPENGL, mFullOutputBinDir + FILE_QT_DLL_OPENGL);

        // Copy the multimedia and network DLLs, if the tools are used
        fileUtil.copy(FILE_QT_DLL_MULTIMEDIA, mFullOutputBinDir + FILE_QT_DLL_MULTIMEDIA);
        fileUtil.copy(FILE_QT_DLL_NETWORK, mFullOutputBinDir + FILE_QT_DLL_NETWORK);
    }

    // --------------------------------------------------------------------------------------------------------------------
    // Step 13: Create the project file (update the references in the .ptp file)
    project.replace(PROJECT_MODULES, projectModules);
    project.replace(PROJECT_OGRE_ROOT, projectOgreRoot);
    project.replace(PROJECT_HEADER, mOutputHeaderDir);
    project.replace(PROJECT_ADDITIONAL_HEADER, projectAdditionalHeader);
    project.replace(PROJECT_SRC, mOutputSrcDir);
    project.replace(PROJECT_ADDITIONAL_SRC, projectAdditionalSrc);
    project.replace(PROJECT_PROJECT, mCurrentProject);
    project.replace(PROJECT_OGRE_INCLUDE, ogreInclude);
    project.replace(PROJECT_OGRE_LIB, ogreLib);
    Utils::writeStringtoFile(project, mOutputDir + mCurrentProjectSlash + mCurrentProject + QString(".pro"));


    // Step 14: Create the mainwindow.h file (update the references in the .htp files)
    mainWindowHeader.replace(MAINWINDOW_INCLUDES, mainWindowAdditionalInclude);
    mainWindowHeader.replace(MAINWINDOW_QT_NAMESPACE, "");
    mainWindowHeader.replace(MAINWINDOW_PUBLIC, mainWindowAdditionalPublic);
    mainWindowHeader.replace(MAINWINDOW_PRIVATE_SLOTS, mainWindowPrivateSlots);
    mainWindowHeader.replace(MAINWINDOW_PRIVATE, mainWindowAdditionalPrivate);
    Utils::writeStringtoFile(mainWindowHeader, mFullOutputHeaderDir + FILE_MAINWINDOW_H);
    //QMessageBox::information(0, "test", mainWindowHeader); // Test

    // --------------------------------------------------------------------------------------------------------------------
    // Step 15: Create the mainwindow.cpp file (update the references in the .ctp files)
    mainWindowSrc.replace(MAINWINDOW_TITLE, mainTitle);
    mainWindowSrc.replace(MAINWINDOW_INCLUDES, "");
    mainWindowSrc.replace(MAINWINDOW_STYLE, applicationTemplate->mStyle + QString(".qss"));
    mainWindowSrc.replace(MAINWINDOW_MAINWINDOW, "");
    mainWindowSrc.replace(MAINWINDOW_OGRE_CONSTRUCTOR, mainWindowOgreConstructor);
    mainWindowSrc.replace(MAINWINDOW_OGRE_DESTRUCTOR, mainWindowOgreDestructor);
    mainWindowSrc.replace(MAINWINDOW_OGRE_UPDATE, mainWindowOgreUpdate);
    mainWindowSrc.replace(MAINWINDOW_OGRE_INITIALIZE, mainWindowOgreInitialize);
    mainWindowSrc.replace(MAINWINDOW_CREATEACTIONS, mainWindowCreateActions);
    mainWindowSrc.replace(MAINWINDOW_CREATEMENUS, mainWindowCreateMenu);
    mainWindowSrc.replace(MAINWINDOW_CREATETOOLBARS, mainWindowCreateToolbars);
    mainWindowSrc.replace(MAINWINDOW_CREATESTATUSBAR, "");
    mainWindowSrc.replace(MAINWINDOW_CREATEDOCKWINDOWS, mainWindowCreateDockWidgets);
    mainWindowSrc.replace(MAINWINDOW_DO_ACTIONS, mainWindowDoActions);
    mainWindowSrc.replace(MAINWINDOW_OGRE_CREATE_WIDGET, mainWindowCreateOgreWidget);
    Utils::writeStringtoFile(mainWindowSrc, mFullOutputSrcDir + FILE_MAINWINDOW_CPP);
}

/****************************************************************************
 Generates generic code for the dockwidget (.h code)
****************************************************************************/
QString QtBuilder::createGenericPrivateMembers(WindowProperties* windowProperties, QString &s)
{
    QString str = s;
    if (windowProperties->mOrder != 0)
    {
        // Define the inner mainwindow (every dockwidget has a mainwindow by default)
        str = str +
            TAB +
            TAB +
            QString("QMainWindow* mInnerMain;") +
            QString("\n");
    }
    return str;
}

/****************************************************************************
 Generates code for the constructor of a dockwidget (.cpp code)
****************************************************************************/
QString QtBuilder::createConstructor(QString &s)
{
    // Define the creation of a mainwindow (to be used for the menu)
    QString str = s;
    str = str +
        TAB +
        QString("mInnerMain = new QMainWindow();") +
        QString("\n");

    // Set the mainwindow in the dock widget
    str = str +
        TAB +
        QString("setWidget(mInnerMain);") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the Transformation widget header files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createTransformationWidgetHeaderForPro(const QString& additionalHeader)
{
    QString str = additionalHeader;
    QFile fileUtil;
    fileUtil.copy(mQtHeader + FILE_TRANSFORMATION_WIDGET_H, mFullOutputHeaderDir + FILE_TRANSFORMATION_WIDGET_H);

    // Add to the project file
    // Headers
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TRANSFORMATION_WIDGET_H +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the Transformation widget cpp files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createTransformationWidgetSrcForPro(const QString& additionalSrc)
{
    QString str = additionalSrc;
    QFile fileUtil;
    fileUtil.copy(mQtSrc + FILE_TRANSFORMATION_WIDGET_CPP, mFullOutputSrcDir + FILE_TRANSFORMATION_WIDGET_CPP);

    // Add to the project file
    // Src
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TRANSFORMATION_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the Asset widget header files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createAssetHeaderForPro(const QString& additionalHeader)
{
    QString str = additionalHeader;
    QFile fileUtil;
    fileUtil.copy(mQtHeader + FILE_ASSET_H, mFullOutputHeaderDir + FILE_ASSET_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_CONSTANTS_H, mFullOutputHeaderDir + FILE_ASSET_CONSTANTS_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_CLICKABLE_FRAME_H, mFullOutputHeaderDir + FILE_ASSET_CLICKABLE_FRAME_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_CLICKABLE_LABEL_H, mFullOutputHeaderDir + FILE_ASSET_CLICKABLE_LABEL_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_CONTAINER_H, mFullOutputHeaderDir + FILE_ASSET_CONTAINER_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_CHECKBOX_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_CHECKBOX_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_STRING_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_STRING_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_XYZ_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_XYZ_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_XY_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_XY_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_SELECT_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_SELECT_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_SLIDER_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_SLIDER_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_DECIMAL_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_DECIMAL_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_COLOR_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_COLOR_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_TEXTURE_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_TEXTURE_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_CURVE_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_CURVE_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_PROPERTY_QUATERNION_H, mFullOutputHeaderDir + FILE_ASSET_PROPERTY_QUATERNION_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_CURVE_GRID_H, mFullOutputHeaderDir + FILE_ASSET_CURVE_GRID_H);
    fileUtil.copy(mQtHeader + FILE_ASSET_CURVE_DIALOG_H, mFullOutputHeaderDir + FILE_ASSET_CURVE_DIALOG_H);

    // Also copy the icons
    fileUtil.copy(mIconDir + ICON_COLLAPSE, mFullOutputIconDir + ICON_COLLAPSE);
    fileUtil.copy(mIconDir + ICON_EXPAND, mFullOutputIconDir + ICON_EXPAND);
    fileUtil.copy(mIconDir + ICON_CURVE_FIT_HOR, mFullOutputIconDir + ICON_CURVE_FIT_HOR);
    fileUtil.copy(mIconDir + ICON_CURVE_FIT_VERT, mFullOutputIconDir + ICON_CURVE_FIT_VERT);
    fileUtil.copy(mIconDir + ICON_CURVE_MOVE, mFullOutputIconDir + ICON_CURVE_MOVE);
    fileUtil.copy(mIconDir + ICON_CURVE_MOVE_ON, mFullOutputIconDir + ICON_CURVE_MOVE_ON);
    fileUtil.copy(mIconDir + ICON_CURVE_RUBBERBAND, mFullOutputIconDir + ICON_CURVE_RUBBERBAND);
    fileUtil.copy(mIconDir + ICON_CURVE_RUBBERBAND_ON, mFullOutputIconDir + ICON_CURVE_RUBBERBAND_ON);
    fileUtil.copy(mIconDir + ICON_CURVE_EDIT, mFullOutputIconDir + ICON_CURVE_EDIT);
    fileUtil.copy(mIconDir + ICON_CURVE_EDIT_ON, mFullOutputIconDir + ICON_CURVE_EDIT_ON);
    fileUtil.copy(mIconDir + ICON_CURVE_ZOOM_IN, mFullOutputIconDir + ICON_CURVE_ZOOM_IN);
    fileUtil.copy(mIconDir + ICON_CURVE_ZOOM_OUT, mFullOutputIconDir + ICON_CURVE_ZOOM_OUT);
    fileUtil.copy(mIconDir + ICON_CURVE_PIVOT, mFullOutputIconDir + ICON_CURVE_PIVOT);
    fileUtil.copy(mIconDir + ICON_CURVE, mFullOutputIconDir + ICON_CURVE);

    // Add to the project file
    // Headers
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_CONSTANTS_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_CLICKABLE_FRAME_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_CLICKABLE_LABEL_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_CONTAINER_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_CHECKBOX_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_STRING_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_XYZ_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_XY_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_SELECT_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_SLIDER_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_DECIMAL_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_COLOR_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_TEXTURE_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_CURVE_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_CURVE_GRID_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_CURVE_DIALOG_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_ASSET_PROPERTY_QUATERNION_H +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the Asset widget cpp files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createAssetSrcForPro(const QString& additionalSrc)
{
    QString str = additionalSrc;
    QFile fileUtil;
    fileUtil.copy(mQtSrc + FILE_ASSET_CPP, mFullOutputSrcDir + FILE_ASSET_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_CONTAINER_CPP, mFullOutputSrcDir + FILE_ASSET_CONTAINER_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_CHECKBOX_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_CHECKBOX_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_STRING_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_STRING_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_XYZ_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_XYZ_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_XY_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_XY_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_SELECT_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_SELECT_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_SLIDER_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_SLIDER_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_DECIMAL_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_DECIMAL_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_COLOR_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_COLOR_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_TEXTURE_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_TEXTURE_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_CURVE_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_CURVE_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_PROPERTY_QUATERNION_CPP, mFullOutputSrcDir + FILE_ASSET_PROPERTY_QUATERNION_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_CURVE_GRID_CPP, mFullOutputSrcDir + FILE_ASSET_CURVE_GRID_CPP);
    fileUtil.copy(mQtSrc + FILE_ASSET_CURVE_DIALOG_CPP, mFullOutputSrcDir + FILE_ASSET_CURVE_DIALOG_CPP);

    // Add to the project file
    // Src
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_CONTAINER_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_CHECKBOX_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_STRING_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_XYZ_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_XY_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_SELECT_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_SLIDER_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_DECIMAL_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_COLOR_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_TEXTURE_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_CURVE_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_CURVE_GRID_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_CURVE_DIALOG_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_ASSET_PROPERTY_QUATERNION_CPP +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the Node editor header files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createNodeHeaderForPro(const QString& additionalHeader)
{
    QString str = additionalHeader;
    QFile fileUtil;
    fileUtil.copy(mQtHeader + FILE_NODE_COMPOUND_H, mFullOutputHeaderDir + FILE_NODE_COMPOUND_H);
    fileUtil.copy(mQtHeader + FILE_NODE_CONNECTION_H, mFullOutputHeaderDir + FILE_NODE_CONNECTION_H);
    fileUtil.copy(mQtHeader + FILE_NODE_CONSTANTS_H, mFullOutputHeaderDir + FILE_NODE_CONSTANTS_H);
    fileUtil.copy(mQtHeader + FILE_NODE_EDITOR_WIDGET_H, mFullOutputHeaderDir + FILE_NODE_EDITOR_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_NODE_H, mFullOutputHeaderDir + FILE_NODE_H);
    fileUtil.copy(mQtHeader + FILE_NODE_PORTTYPE_H, mFullOutputHeaderDir + FILE_NODE_PORTTYPE_H);
    fileUtil.copy(mQtHeader + FILE_NODE_PORT_H, mFullOutputHeaderDir + FILE_NODE_PORT_H);
    fileUtil.copy(mQtHeader + FILE_NODE_SCENE_H, mFullOutputHeaderDir + FILE_NODE_SCENE_H);

    // Also copy the icons
    fileUtil.copy(mIconDir + ICON_CLOSE, mFullOutputIconDir + ICON_CLOSE);
    fileUtil.copy(mIconDir + ICON_MINMAX, mFullOutputIconDir + ICON_MINMAX);
    fileUtil.copy(mIconDir + ICON_COMPOUND, mFullOutputIconDir + ICON_COMPOUND);

    // Add to the project file
    // Headers
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_COMPOUND_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_CONNECTION_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_CONSTANTS_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_EDITOR_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_PORTTYPE_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_PORT_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_NODE_SCENE_H +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the Node editor cpp files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createNodeSrcForPro(const QString& additionalSrc)
{
    QString str = additionalSrc;
    QFile fileUtil;
    fileUtil.copy(mQtSrc + FILE_NODE_COMPOUND_CPP, mFullOutputSrcDir + FILE_NODE_COMPOUND_CPP);
    fileUtil.copy(mQtSrc + FILE_NODE_CONNECTION_CPP, mFullOutputSrcDir + FILE_NODE_CONNECTION_CPP);
    fileUtil.copy(mQtSrc + FILE_NODE_CPP, mFullOutputSrcDir + FILE_NODE_CPP);
    fileUtil.copy(mQtSrc + FILE_NODE_EDITOR_WIDGET_CPP, mFullOutputSrcDir + FILE_NODE_EDITOR_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_NODE_PORTTYPE_CPP, mFullOutputSrcDir + FILE_NODE_PORTTYPE_CPP);
    fileUtil.copy(mQtSrc + FILE_NODE_PORT_CPP, mFullOutputSrcDir + FILE_NODE_PORT_CPP);

    // Add to the project file
    // Src
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_NODE_COMPOUND_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_NODE_CONNECTION_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_NODE_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_NODE_EDITOR_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_NODE_PORTTYPE_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_NODE_PORT_CPP +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the 'Tool' header files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createToolHeaderForPro(const QString& additionalHeader)
{
    QString str = additionalHeader;
    QFile fileUtil;
    fileUtil.copy(mQtHeader + FILE_MAGUS_TREE_WIDGET_H, mFullOutputHeaderDir + FILE_MAGUS_TREE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_GRADIENT_H, mFullOutputHeaderDir + FILE_TOOL_GRADIENT_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_GRADIENT_MARKER_H, mFullOutputHeaderDir + FILE_TOOL_GRADIENT_MARKER_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_GRADIENT_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_GRADIENT_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_SIMPLE_TEXTURE_MODEL_H, mFullOutputHeaderDir + FILE_TOOL_SIMPLE_TEXTURE_MODEL_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_SIMPLE_TEXTURE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_SIMPLE_TEXTURE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_DEFAULT_TEXTURE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_DEFAULT_TEXTURE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_EXTENDED_TEXTURE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_EXTENDED_TEXTURE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_RESOURCETREE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_RESOURCETREE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_SCENE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_SCENE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_LAYERED_SCENE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_LAYERED_SCENE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_LAYER_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_LAYER_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_GL_SPHERE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_GL_SPHERE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_FILEREADER_H, mFullOutputHeaderDir + FILE_TOOL_FILEREADER_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_AUDIO_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_AUDIO_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_GENERIC_ASSET_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_GENERIC_ASSET_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_RESOURCE_WIDGET_H, mFullOutputHeaderDir + FILE_TOOL_RESOURCE_WIDGET_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_RESOURCE_ASSETS_H, mFullOutputHeaderDir + FILE_TOOL_RESOURCE_ASSETS_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_RESOURCE_COLLECTIONS_H, mFullOutputHeaderDir + FILE_TOOL_RESOURCE_COLLECTIONS_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_RESOURCE_MAIN_H, mFullOutputHeaderDir + FILE_TOOL_RESOURCE_MAIN_H);
    fileUtil.copy(mQtHeader + FILE_TOOL_RESOURCE_SOURCES_H, mFullOutputHeaderDir + FILE_TOOL_RESOURCE_SOURCES_H);

    // Also copy the icons
    fileUtil.copy(mIconDir + ICON_CLOSE_BOLD, mFullOutputIconDir + ICON_CLOSE_BOLD);
    fileUtil.copy(mIconDir + ICON_LAYER_BOLD, mFullOutputIconDir + ICON_LAYER_BOLD);
    fileUtil.copy(mIconDir + ICON_VISIBLE_BOLD, mFullOutputIconDir + ICON_VISIBLE_BOLD);
    fileUtil.copy(mIconDir + ICON_INVISIBLE_BOLD, mFullOutputIconDir + ICON_INVISIBLE_BOLD);
    fileUtil.copy(mIconDir + ICON_SEARCH_BOLD, mFullOutputIconDir + ICON_SEARCH_BOLD);
    fileUtil.copy(mIconDir + ICON_ASSET_AUDIO_PLAY, mFullOutputIconDir + ICON_ASSET_AUDIO_PLAY);
    fileUtil.copy(mIconDir + ICON_ASSET_AUDIO_PAUSE, mFullOutputIconDir + ICON_ASSET_AUDIO_PAUSE);
    fileUtil.copy(mIconDir + ICON_ASSET_AUDIO_STOP, mFullOutputIconDir + ICON_ASSET_AUDIO_STOP);
    fileUtil.copy(mIconDir + ICON_ASSET_MATERIAL, mFullOutputIconDir + ICON_ASSET_MATERIAL);
    fileUtil.copy(mIconDir + ICON_ASSET_MESH, mFullOutputIconDir + ICON_ASSET_MESH);
    fileUtil.copy(mIconDir + ICON_ASSET_SCRIPT, mFullOutputIconDir + ICON_ASSET_SCRIPT);
    fileUtil.copy(mIconDir + ICON_AUDIO, mFullOutputIconDir + ICON_AUDIO);
    fileUtil.copy(mIconDir + ICON_MATERIAL, mFullOutputIconDir + ICON_MATERIAL);
    fileUtil.copy(mIconDir + ICON_SOFTBODY, mFullOutputIconDir + ICON_SOFTBODY);
    fileUtil.copy(mIconDir + ICON_TEXTURE, mFullOutputIconDir + ICON_TEXTURE);
    fileUtil.copy(mIconDir + ICON_COG, mFullOutputIconDir + ICON_COG);

    // Add to the project file
    // Headers
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_MAGUS_TREE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_GRADIENT_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_GRADIENT_MARKER_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_GRADIENT_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_SIMPLE_TEXTURE_MODEL_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_SIMPLE_TEXTURE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_DEFAULT_TEXTURE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_EXTENDED_TEXTURE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_RESOURCETREE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_SCENE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_LAYERED_SCENE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_LAYER_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_GL_SPHERE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_FILEREADER_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_AUDIO_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_GENERIC_ASSET_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_RESOURCE_WIDGET_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_RESOURCE_ASSETS_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_RESOURCE_COLLECTIONS_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_RESOURCE_MAIN_H +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputHeaderDir +
        FILE_TOOL_RESOURCE_SOURCES_H +
        QString(" \\ ") +
        QString("\n");

    return str;
}

/****************************************************************************
 Generates code to copy the 'Node editor 'Tool' cpp files and add
 them to the project (.pro) file.
****************************************************************************/
QString QtBuilder::createToolSrcForPro(const QString& additionalSrc)
{
    QString str = additionalSrc;
    QFile fileUtil;
    fileUtil.copy(mQtSrc + FILE_MAGUS_TREE_WIDGET_CPP, mFullOutputSrcDir + FILE_MAGUS_TREE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_GRADIENT_CPP, mFullOutputSrcDir + FILE_TOOL_GRADIENT_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_GRADIENT_MARKER_CPP, mFullOutputSrcDir + FILE_TOOL_GRADIENT_MARKER_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_GRADIENT_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_GRADIENT_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_SIMPLE_TEXTURE_MODEL_CPP, mFullOutputSrcDir + FILE_TOOL_SIMPLE_TEXTURE_MODEL_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_SIMPLE_TEXTURE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_SIMPLE_TEXTURE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_DEFAULT_TEXTURE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_DEFAULT_TEXTURE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_EXTENDED_TEXTURE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_EXTENDED_TEXTURE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_RESOURCETREE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_RESOURCETREE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_SCENE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_SCENE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_LAYERED_SCENE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_LAYERED_SCENE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_LAYER_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_LAYER_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_GL_SPHERE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_GL_SPHERE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_FILEREADER_CPP, mFullOutputSrcDir + FILE_TOOL_FILEREADER_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_AUDIO_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_AUDIO_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_GENERIC_ASSET_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_GENERIC_ASSET_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_RESOURCE_WIDGET_CPP, mFullOutputSrcDir + FILE_TOOL_RESOURCE_WIDGET_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_RESOURCE_ASSETS_CPP, mFullOutputSrcDir + FILE_TOOL_RESOURCE_ASSETS_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_RESOURCE_COLLECTIONS_CPP, mFullOutputSrcDir + FILE_TOOL_RESOURCE_COLLECTIONS_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_RESOURCE_MAIN_CPP, mFullOutputSrcDir + FILE_TOOL_RESOURCE_MAIN_CPP);
    fileUtil.copy(mQtSrc + FILE_TOOL_RESOURCE_SOURCES_CPP, mFullOutputSrcDir + FILE_TOOL_RESOURCE_SOURCES_CPP);

    // Add to the project file
    // Src
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_MAGUS_TREE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_GRADIENT_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_GRADIENT_MARKER_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_GRADIENT_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_SIMPLE_TEXTURE_MODEL_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_SIMPLE_TEXTURE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_DEFAULT_TEXTURE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_EXTENDED_TEXTURE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_RESOURCETREE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_SCENE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_LAYERED_SCENE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_LAYER_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_GL_SPHERE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_FILEREADER_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_AUDIO_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_GENERIC_ASSET_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_RESOURCE_WIDGET_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_RESOURCE_ASSETS_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_RESOURCE_COLLECTIONS_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_RESOURCE_MAIN_CPP +
        QString(" \\ ") +
        QString("\n");
    str = str +
        TAB +
        mOutputSrcDir +
        FILE_TOOL_RESOURCE_SOURCES_CPP +
        QString(" \\ ") +
        QString("\n");

    return str;
}

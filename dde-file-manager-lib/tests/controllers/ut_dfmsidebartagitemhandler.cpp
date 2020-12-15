#include "gtest/gtest.h"
#include "gmock/gmock-matchers.h"

#include "dfmstandardpaths.h"
#include <QProcess>
#include "tag/tagmanager.h"
#include "views/dfilemanagerwindow.h"
#include "interfaces/dfmsidebaritem.h"

#include "stub.h"

#define private public
#define protected public

#include "controllers/dfmsidebartagitemhandler.h"

DFM_USE_NAMESPACE

using namespace testing;
namespace  {
    class TestDFMSideBarTagItemHandler : public Test
    {
    public:
        TestDFMSideBarTagItemHandler():Test()
        {
            m_handler = nullptr;
        }

        virtual void SetUp() override {
            m_handler = new DFMSideBarTagItemHandler();
        }

        virtual void TearDown() override {
            delete  m_handler;
        }

        DFMSideBarTagItemHandler *m_handler;
    };
}

TEST_F(TestDFMSideBarTagItemHandler, tst_createItem)
{ 
    QString testFile = DFMStandardPaths::location(DFMStandardPaths::PicturesPath) + "/utFile";

    QString cmdTouch = QString("touch ") + testFile;
    QString cmdRm = QString("rm ") + testFile;

    QProcess::execute(cmdTouch);

    QString (*st_getTagIconName)(void *, const QString &) = [](void *,const QString &){
        return QString();
    };

    Stub stub;
    stub.set((QString(TagManager::*)(const QString &)const)ADDR(TagManager, getTagIconName), st_getTagIconName);

    DUrl url(testFile);
    EXPECT_NO_FATAL_FAILURE(m_handler->createItem(url));
}

TEST_F(TestDFMSideBarTagItemHandler, tst_contextMenu)
{
    Stub stub;
    static bool myCallOpen = false;
    void (*ut_openNewTab)() = [](){myCallOpen = true;};
    stub.set(ADDR(DFileManagerWindow, openNewTab), ut_openNewTab);

    DFileManagerWindow window;
    EXPECT_TRUE(myCallOpen);

    const DFMSideBar *bar = window.getLeftSideBar();

    DFMSideBarItem *item = DFMSideBarItem::createSeparatorItem(QString("Trash"));
    ASSERT_NE(item, nullptr);

    QMenu *menu = m_handler->contextMenu(bar, item);
    QList<QAction *> actions = menu->actions();
    for (auto action : actions) {
        action->trigger();
    }

    EXPECT_NE(menu, nullptr);

    delete item;
    delete menu;
}

TEST_F(TestDFMSideBarTagItemHandler, tst_rename)
{
    Stub stub;
    static bool myCallOpen = false;
    void (*ut_openNewTab)() = [](){myCallOpen = true;};
    stub.set(ADDR(DFileManagerWindow, openNewTab), ut_openNewTab);

    DFileManagerWindow window;
    EXPECT_TRUE(myCallOpen);

    DFMSideBarItem *item = DFMSideBarItem::createSeparatorItem(QString("Trash"));
    ASSERT_NE(item, nullptr);

    EXPECT_NO_FATAL_FAILURE(m_handler->rename(item, "newName"));

    delete item;
}
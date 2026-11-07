// SPDX-License-Identifier: GPL-3.0-only
/*
 * XLuke44's Prismlauncher — custom AMOLED theme for Prism Launcher 8.4.
 * This file is part of XLuke44's Prismlauncher.
 */
#include "AMOLEDTheme.h"

#include <QObject>

QString AMOLEDTheme::id()
{
    return "amoled";
}

QString AMOLEDTheme::name()
{
    return QObject::tr("AMOLED Green");
}

bool AMOLEDTheme::hasColorScheme()
{
    return true;
}

QPalette AMOLEDTheme::colorScheme()
{
    // Keep the OLED canvas truly black while using subtle green-tinted layers
    // for controls, panels and alternate rows. The accent is intentionally a
    // soft leaf green rather than a high-luminance neon green.
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#000000"));
    palette.setColor(QPalette::WindowText, QColor("#dce8de"));
    palette.setColor(QPalette::Base, QColor("#060a07"));
    palette.setColor(QPalette::AlternateBase, QColor("#0c140e"));
    palette.setColor(QPalette::ToolTipBase, QColor("#111b14"));
    palette.setColor(QPalette::ToolTipText, QColor("#e3eee5"));
    palette.setColor(QPalette::Text, QColor("#dce8de"));
    palette.setColor(QPalette::Button, QColor("#0e1710"));
    palette.setColor(QPalette::ButtonText, QColor("#dce8de"));
    palette.setColor(QPalette::BrightText, QColor("#e58a8a"));
    palette.setColor(QPalette::Link, QColor("#81b98a"));
    palette.setColor(QPalette::LinkVisited, QColor("#789b80"));
    palette.setColor(QPalette::Highlight, QColor("#28543a"));
    palette.setColor(QPalette::HighlightedText, QColor("#f0f5f1"));
    palette.setColor(QPalette::PlaceholderText, QColor("#78867b"));
    palette.setColor(QPalette::Light, QColor("#293b2e"));
    palette.setColor(QPalette::Midlight, QColor("#1a2a1e"));
    palette.setColor(QPalette::Dark, QColor("#000000"));
    palette.setColor(QPalette::Mid, QColor("#111c14"));
    palette.setColor(QPalette::Shadow, QColor("#000000"));
    return fadeInactive(palette, fadeAmount(), fadeColor());
}

double AMOLEDTheme::fadeAmount()
{
    return 0.32;
}

QColor AMOLEDTheme::fadeColor()
{
    return QColor("#000000");
}

bool AMOLEDTheme::hasStyleSheet()
{
    return true;
}

QString AMOLEDTheme::appStyleSheet()
{
    return QStringLiteral(R"QSS(
        /* Shared foreground and selection colors. Leave widget backgrounds to
           the palette unless a component needs its own visual surface. */
        QWidget {
            color: #dce8de;
            selection-background-color: #28543a;
            selection-color: #f0f5f1;
        }

        QMainWindow, QDialog {
            background-color: #000000;
        }
        QMenuBar, QStatusBar, QToolBar {
            background-color: #050906;
        }
        QToolBar {
            border: none;
            border-bottom: 1px solid #142019;
            spacing: 6px;
            padding: 3px;
        }
        QToolBar::separator {
            background-color: #26392b;
            width: 1px;
            margin: 5px 6px;
        }
        QMenuBar::item {
            background: transparent;
            padding: 5px 9px;
            border-radius: 4px;
        }
        QMenuBar::item:selected { background-color: #142219; }
        QMenuBar::item:pressed { background-color: #1d3825; }
        QStatusBar::item { border: none; }
        QTabWidget::pane {
            background-color: #000000;
            border: 1px solid #18251c;
        }

        QMenu {
            background-color: #080e0a;
            color: #dce8de;
            border: 1px solid #263b2c;
            border-radius: 5px;
            padding: 4px;
        }
        QMenu::item {
            padding: 6px 28px 6px 10px;
            border-radius: 3px;
            background: transparent;
        }
        QMenu::item:selected {
            background-color: #1b3825;
            color: #eff5f0;
        }
        QMenu::item:disabled { color: #657268; }
        QMenu::separator {
            height: 1px;
            background: #1e3023;
            margin: 5px 8px;
        }

        QPushButton, QToolButton {
            background-color: #0e1710;
            color: #dce8de;
            border: 1px solid #294332;
            border-radius: 6px;
            padding: 6px 10px;
        }
        QPushButton:hover, QToolButton:hover {
            background-color: #15251a;
            border-color: #527d5c;
        }
        QPushButton:pressed, QToolButton:pressed {
            background-color: #1d3b27;
            border-color: #659672;
        }
        QPushButton:checked, QToolButton:checked {
            background-color: #1a3523;
            border-color: #6c9f77;
            color: #e3eee5;
        }
        QPushButton:disabled, QToolButton:disabled {
            background-color: #070b08;
            color: #657268;
            border-color: #1a251d;
        }

        QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QDoubleSpinBox,
        QComboBox, QDateEdit, QTimeEdit, QDateTimeEdit {
            background-color: #060b07;
            color: #dce8de;
            border: 1px solid #263a2c;
            border-radius: 5px;
            padding: 5px 7px;
            selection-background-color: #28543a;
            selection-color: #f0f5f1;
        }
        QLineEdit:hover, QTextEdit:hover, QPlainTextEdit:hover,
        QSpinBox:hover, QDoubleSpinBox:hover, QComboBox:hover,
        QDateEdit:hover, QTimeEdit:hover, QDateTimeEdit:hover {
            border-color: #3b5a42;
        }
        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus,
        QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus,
        QDateEdit:focus, QTimeEdit:focus, QDateTimeEdit:focus {
            border: 1px solid #6d9f77;
        }
        QLineEdit:disabled, QTextEdit:disabled, QPlainTextEdit:disabled,
        QSpinBox:disabled, QDoubleSpinBox:disabled, QComboBox:disabled {
            background-color: #050806;
            color: #657268;
            border-color: #18231b;
        }
        QComboBox::drop-down {
            background: transparent;
            border: none;
            width: 22px;
        }
        QComboBox QAbstractItemView {
            background-color: #080e0a;
            color: #dce8de;
            selection-background-color: #28543a;
            selection-color: #f0f5f1;
            border: 1px solid #294332;
            outline: 0;
        }

        QCheckBox, QRadioButton {
            spacing: 8px;
            background: transparent;
        }
        QCheckBox::indicator, QRadioButton::indicator {
            width: 15px;
            height: 15px;
            background-color: #080e0a;
            border: 1px solid #526b58;
        }
        QCheckBox::indicator { border-radius: 4px; }
        QRadioButton::indicator { border-radius: 8px; }
        QCheckBox::indicator:hover, QRadioButton::indicator:hover {
            border-color: #80b58a;
        }
        QCheckBox::indicator:checked, QRadioButton::indicator:checked {
            background-color: #6fa77b;
            border: 1px solid #83b98d;
        }
        QCheckBox::indicator:disabled, QRadioButton::indicator:disabled {
            background-color: #080c09;
            border-color: #29372d;
        }

        QTabBar::tab {
            background-color: #080e0a;
            color: #aab9ad;
            border: 1px solid #1c2c20;
            padding: 7px 13px;
        }
        QTabBar::tab:hover {
            background-color: #101c13;
            color: #dce8de;
        }
        QTabBar::tab:selected {
            background-color: #14251a;
            color: #e3eee5;
            border-color: #294332;
            border-bottom: 2px solid #78ad82;
        }
        QTabBar::tab:disabled { color: #626e65; }

        QHeaderView::section {
            background-color: #0c150e;
            color: #dce8de;
            border: 1px solid #1d2d21;
            padding: 6px 7px;
        }
        QTreeView, QListView, QTableView, QTableWidget, QColumnView {
            background-color: #000000;
            alternate-background-color: #080f0a;
            color: #dce8de;
            gridline-color: #1b2a1f;
            selection-background-color: #28543a;
            selection-color: #f0f5f1;
            outline: 0;
        }
        QTreeView::item, QListView::item, QTableView::item,
        QTableWidget::item, QColumnView::item {
            padding: 3px;
        }
        QTreeView::item:hover, QListView::item:hover,
        QTableView::item:hover, QTableWidget::item:hover,
        QColumnView::item:hover {
            background-color: #101c13;
        }
        QTreeView::item:selected, QListView::item:selected,
        QTableView::item:selected, QTableWidget::item:selected,
        QColumnView::item:selected {
            background-color: #28543a;
            color: #f0f5f1;
        }
        QTableCornerButton::section {
            background-color: #0c150e;
            border: 1px solid #1d2d21;
        }

        QGroupBox {
            background: transparent;
            border: 1px solid #1e3023;
            border-radius: 6px;
            margin-top: 9px;
            padding-top: 9px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 9px;
            padding: 0 5px;
            color: #a9c4ae;
        }

        QProgressBar {
            background: #080f0a;
            color: #dce8de;
            border: 1px solid #203629;
            border-radius: 5px;
            text-align: center;
            padding: 1px;
        }
        QProgressBar::chunk {
            background-color: #477d53;
            border-radius: 3px;
        }

        QSlider::groove:horizontal {
            height: 5px;
            background: #1a2a1e;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            width: 14px;
            margin: -5px 0;
            background: #6fa77b;
            border: 1px solid #83b98d;
            border-radius: 7px;
        }
        QSlider::handle:horizontal:hover { background: #80b58a; }

        QScrollBar:vertical, QScrollBar:horizontal {
            background: #000000;
            border: none;
            margin: 0;
        }
        QScrollBar:vertical { width: 10px; }
        QScrollBar:horizontal { height: 10px; }
        QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
            background: #263a2c;
            border-radius: 4px;
            min-height: 25px;
            min-width: 25px;
        }
        QScrollBar::handle:hover { background: #3d6548; }
        QScrollBar::add-line, QScrollBar::sub-line {
            background: #000000;
            border: none;
            width: 0;
            height: 0;
        }
        QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

        QToolTip {
            color: #e3eee5;
            background-color: #111b14;
            border: 1px solid #45694e;
            border-radius: 4px;
            padding: 5px 7px;
        }
        QSplitter::handle { background-color: #17251b; }
        QSplitter::handle:hover { background-color: #426b4b; }
        QStatusBar::item { border: none; }
    )QSS");
}

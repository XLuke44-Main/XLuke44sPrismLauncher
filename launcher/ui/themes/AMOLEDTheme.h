// SPDX-License-Identifier: GPL-3.0-only
/*
 * XLuke44's Prismlauncher — custom AMOLED theme for Prism Launcher 8.4.
 * This file is part of XLuke44's Prismlauncher.
 */
#pragma once

#include "FusionTheme.h"

class AMOLEDTheme : public FusionTheme {
   public:
    ~AMOLEDTheme() override = default;

    QString id() override;
    QString name() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    bool hasColorScheme() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};

//  Copyright 2013 Guillaume Denry (guillaume.denry@gmail.com)
//  This file is part of BetaSeeker.
//
//  BetaSeeker is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  BetaSeeker is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with BetaSeeker.  If not, see <http://www.gnu.org/licenses/>.

#include <QFileDialog>

#include "seasonwidget.h"
#include "settings.h"

#include "ui_showdetailwidget.h"
#include "showdetailwidget.h"

ShowDetailWidget::ShowDetailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDetailWidget)
{
    ui->setupUi(this);

    seasonWidget = new SeasonWidget;
    ui->scrollArea->setWidget(seasonWidget);
}

void ShowDetailWidget::init(const QString &showId, int season)
{
    _showId = showId;
	_season = season;
    seasonWidget->init(_showId, _season);

    ui->widgetSeasonDir->setVisible(Settings::directoryForSeason(_showId, _season).isEmpty());
}

void ShowDetailWidget::on_pushButtonDefineIt_clicked()
{
    QString filePath = QFileDialog::getExistingDirectory(this, tr("Pick your show season directory"));
    if (filePath.isEmpty())
        return;
    Settings::setDirectoryForSeason(_showId, _season, filePath);
    ui->widgetSeasonDir->hide();
}

void ShowDetailWidget::on_pushButtonForgetIt_clicked()
{
    ui->widgetSeasonDir->hide();
}

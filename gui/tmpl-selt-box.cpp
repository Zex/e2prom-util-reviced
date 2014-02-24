/*
 * tmpl-selt-box.cpp
 *
 * Author: Zex.Li <zex@hytera.com>
 */
#include <tmpl-selt-box.h>

QT_BEGIN_NAMESPACE

tmpl_selt_box::tmpl_selt_box(QWidget *parent)
{
	setup_gui(parent);
}

void tmpl_selt_box::setup_gui(QWidget *parent)
{
	select_template_label = new QLabel(parent);
	select_template_label->setGeometry(QRect(10, 30, 121, 23));
	select_template_label->setText(QApplication::translate("E2prom Util", "Select a template", 0, QApplication::UnicodeUTF8));
	select_template_cmbox = new QComboBox(parent);
	select_template_cmbox->setGeometry(QRect(140, 30, 121, 27));

	select_template_cmbox->clear();
	select_template_cmbox->insertItems(0, QStringList()
		 << QApplication::translate("E2prom Util", "fanu_eeprom", 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("E2prom Util", "abu_eeprom", 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("E2prom Util", "bcb_eeprom", 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("E2prom Util", "bscu_eeprom", 0, QApplication::UnicodeUTF8)
		 << QApplication::translate("E2prom Util", "dib-r5_eeprom", 0, QApplication::UnicodeUTF8)
		);
}

QT_END_NAMESPACE


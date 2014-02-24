/*
 * parameter_panel.cpp
 *
 * Author: Zex.Li <zex@hytera.com>
 */
#include <parameter-panel.h>

QT_BEGIN_NAMESPACE

parameter_panel::parameter_panel(QWidget *parent)
: x_cur(10), y_cur(70), y_pri(0), w_cur(0), h_cur(0)
{
	setup_gui(parent);
}

void parameter_panel::update_panel_(int start, int show_end, QStringList lb_names)
{
	int i = start, j = start, y_gap = 40, y_tmp = y_pri;
	QString name;

	if (lb_names.size() < (show_end/2)) return;

	for (; i < show_end; i++) {

		if (!(i % 2)) {
			name = lb_names[j]+" Serial Number";
		} else {
			name = lb_names[j++]+" Product Date";
			y_tmp += y_gap;
		}

		param_names[i]->setText(QApplication::translate("E2prom Util", 
			name.toStdString().c_str(), 0, QApplication::UnicodeUTF8));

		param_names[i]->show();
		param_values[i]->show();

	}

	for (; i < 16; i += 2) {

		/* serial number */
		param_names[i]->hide();
		param_values[i]->hide();

		/* product number */
		param_names[i+1]->hide();
		param_values[i+1]->hide();
	}

	y_cur = y_tmp;
}

/* ./temp2bin fan_temp.txt pi sn pd ti */
void parameter_panel::show_panel_fanu()
{
	/* hide all extern parameters */
	update_panel_(0, 0, QStringList());
}

/* ./temp2bin abu_temp.txt pi sn pd ti sn1 pd1 */
void parameter_panel::show_panel_abu()
{
	QStringList lb_names;

	lb_names += "Alarm Board";
	/* show sn1, pd1 */
	update_panel_(0, 2, lb_names);
}

/* ./temp2bin bscu_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 */
void parameter_panel::show_panel_bscu()
{
	QStringList lb_names;

	lb_names += "Main Board";
	lb_names += "GNSS Board";
	lb_names += "MCB";
	/* show sn1, pd1, sn2, pd2, sn3, pd3 */
	update_panel_(0, 6, lb_names);
}

/* ./temp2bin bcb_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 */
void parameter_panel::show_panel_bcb()
{
	QStringList lb_names;

	lb_names += "BCB";
	lb_names += "LPS";
	lb_names += "DPA";
	/* show sn1, pd1, sn2, pd2, sn3, pd3 */
	update_panel_(0, 6, lb_names);
}

/* ./temp2bin dibr5_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 sn4 pd4 sn5 pd5 sn6 pd6 sn7 pd7 sn8 pd8 */
void parameter_panel::show_panel_dibr5()
{
	/* show sn1, pd1, sn2, pd2, sn3, pd3,
	 * sn4, pd4, sn5, pd5, sn6, pd6, sn7
	 * pd7, sn8, pd8 */
	QStringList lb_names;

	lb_names += "BSCU1";
	lb_names += "BSCU2";
	lb_names += "CHU1";
	lb_names += "CHU2";
	lb_names += "CHU3";
	lb_names += "CHU4";
	lb_names += "Fan Unit";
	lb_names += "Alarm Box";
	update_panel_(0, 16, lb_names);
}

void parameter_panel::init_pri_param_(QWidget *parent)
{
	pi_label = new QLabel(parent);
	pi_label->setGeometry(QRect(10, y_cur, 91, 23));
	pi_label->setText(QApplication::translate("E2prom Util", "Product Index", 0, QApplication::UnicodeUTF8));
	pi_ledit = new QLineEdit(parent);
	pi_ledit->setGeometry(QRect(110, y_cur, 71, 23));

	sn_label = new QLabel(parent);
	sn_label->setGeometry(QRect(190, y_cur, 100, 23));
	sn_label->setText(QApplication::translate("E2prom Util", "Serial Number", 0, QApplication::UnicodeUTF8));
	sn_ledit = new QLineEdit(parent);
	sn_ledit->setGeometry(QRect(300, y_cur, 81, 23));

	pd_label = new QLabel(parent);
	pd_label->setGeometry(QRect(390, y_cur, 100, 23));
	pd_label->setText(QApplication::translate("E2prom Util", "Product Date", 0, QApplication::UnicodeUTF8));
	pd_ledit = new QLineEdit(parent);
	pd_ledit->setGeometry(QRect(490, y_cur, 113, 23));

	ti_label = new QLabel(parent);
	ti_label->setGeometry(QRect(620, y_cur, 100, 23));
	ti_label->setText(QApplication::translate("E2prom Util", "Test Instruction", 0, QApplication::UnicodeUTF8));
	ti_ledit = new QLineEdit(parent);
	ti_ledit->setGeometry(QRect(730, y_cur, 71, 23));

	y_cur += 23;
	y_pri = y_cur;

	w_cur += 730+71;
	h_cur += y_cur+23;
}

void parameter_panel::init_ext_param_(QWidget *parent)
{
	int x_col[] = {10, 200, 300, 490}, y_gap = 40, y_current = 110;

	for (int i = 0; i < 16; i += 2) {

		/* ---------------one line begin-----------------------*/
		param_names.append(new QLabel(parent));
		param_names.append(new QLabel(parent));
		param_values.append(new QLineEdit(parent));
		param_values.append(new QLineEdit(parent));

		/* serial number */
		param_names[i]->setGeometry(QRect(x_col[0], y_current, 181, 23));
		param_names[i]->setText(QApplication::translate("E2prom Util", "Sub Serial Number", 0, QApplication::UnicodeUTF8));
		param_names[i]->hide();

		param_values[i]->setGeometry(QRect(x_col[1], y_current, 81, 23));
		param_values[i]->hide();

		/* product date */
		param_names[i+1]->setGeometry(QRect(x_col[2], y_current, 181, 23));
		param_names[i+1]->setText(QApplication::translate("E2prom Util", "Sub Product Date", 0, QApplication::UnicodeUTF8));
		param_names[i+1]->hide();

		param_values[i+1]->setGeometry(QRect(x_col[3], y_current, 113, 23));
		param_values[i+1]->hide();
		/* ------------------one line end----------------------*/
	
		y_current += y_gap;
	}
}

QStringList parameter_panel::collect_pri_params_(QString sn_tail)
{
	QStringList ret;

	ret.append(pi_ledit->text());
	ret.append(sn_ledit->text()+sn_tail);
	ret.append(pd_ledit->text());
	ret.append(ti_ledit->text());

	return ret;
}

QStringList parameter_panel::collect_ext_params_(int start, int end, QString sn_tail)
{
	int i = start;
	QStringList ret;

	for (; i < end; i += 2) {

		ret.append(param_values[i]->text()+sn_tail);
		ret.append(param_values[i+1]->text());
	}

	return ret;
}

/* ./temp2bin fan_temp.txt pi sn pd ti */
QStringList parameter_panel::collect_params_fanu()
{
	return collect_pri_params_("/000");
}

/* ./temp2bin abu_temp.txt pi sn pd ti sn1 pd1 */
QStringList parameter_panel::collect_params_abu()
{
	QStringList ret;

	ret = collect_pri_params_("/000");
	ret += collect_ext_params_(0, 2, "/000");

	return ret;
}

/* ./temp2bin bscu_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 */
QStringList parameter_panel::collect_params_bscu()
{
	QStringList ret;

	ret = collect_pri_params_("/002");
	ret += collect_ext_params_(0, 6, "/002");

	return ret;
}

/* ./temp2bin bcb_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 */
QStringList parameter_panel::collect_params_bcb()
{
	QStringList ret;

	ret = collect_pri_params_("/002");
	ret += collect_ext_params_(0, 6, "/002");

	return ret;
}

/* ./temp2bin dibr5_temp.txt pi sn pd ti sn1 pd1 sn2 pd2 sn3 pd3 sn4 pd4 sn5 pd5 sn6 pd6 sn7 pd7 sn8 pd8 */
QStringList parameter_panel::collect_params_dibr5()
{
	QStringList ret;

	ret = collect_pri_params_("/002");
	ret += collect_ext_params_(0, 16, "/002");

	return ret;
}

void parameter_panel::setup_gui(QWidget *parent)
{
	init_pri_param_(parent);
	init_ext_param_(parent);
}



//parameter_panel_dibr5::parameter_panel_dibr5(QWidget *parent)
//: parameter_panel_general(parent)
//{
//	setup_gui(parent);
//}
//
//QStringList parameter_panel_dibr5::collect_params()
//{
//	QStringList argv;
//
////	argv = {
//	argv.append(pi_ledit->text());
//	argv.append(sn_ledit->text());
//	argv.append(pd_ledit->text());
//	argv.append(ti_ledit->text());
//
//	argv.append(sn_ledit_bscu1->text());
//	argv.append(pd_ledit_bscu1->text());
//	argv.append(sn_ledit_bscu2->text());
//	argv.append(pd_ledit_bscu2->text());
//
//	argv.append(sn_ledit_chu1->text());
//	argv.append(pd_ledit_chu1->text());
//	argv.append(sn_ledit_chu2->text());
//	argv.append(pd_ledit_chu2->text());
//	argv.append(sn_ledit_chu3->text());
//	argv.append(pd_ledit_chu3->text());
//	argv.append(sn_ledit_chu4->text());
//	argv.append(pd_ledit_chu4->text());
//
//	argv.append(sn_ledit_fanu->text());
//	argv.append(pd_ledit_fanu->text());
//	argv.append(sn_ledit_albox->text());
//	argv.append(pd_ledit_albox->text());
////	};
//
//	return argv;
//}
//
//void parameter_panel_dibr5::setup_gui(QWidget *parent)
//{
//	int x_col[] = {10, 200, 300, 490}, y_gap = 40, y_current = 110;
//
//	sn_label_bscu1 = new QLabel(parent);
//	sn_label_bscu1->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_bscu1->setText(QApplication::translate("E2prom Util", "BSCU1 Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_bscu1 = new QLineEdit(parent);
//	sn_ledit_bscu1->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_bscu1 = new QLabel(parent);
//	pd_label_bscu1->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_bscu1->setText(QApplication::translate("E2prom Util", "BSCU1 Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_bscu1 = new QLineEdit(parent);
//	pd_ledit_bscu1->setGeometry(QRect(x_col[3], y_current, 113, 23));
//	
//	y_current += y_gap;
//
//	sn_label_bscu2 = new QLabel(parent);
//	sn_label_bscu2->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_bscu2->setText(QApplication::translate("E2prom Util", "BSCU2 Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_bscu2 = new QLineEdit(parent);
//	sn_ledit_bscu2->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_bscu2 = new QLabel(parent);
//	pd_label_bscu2->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_bscu2->setText(QApplication::translate("E2prom Util", "BSCU2 Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_bscu2 = new QLineEdit(parent);
//	pd_ledit_bscu2->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_chu1 = new QLabel(parent);
//	sn_label_chu1->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_chu1->setText(QApplication::translate("E2prom Util", "CHU1 Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_chu1 = new QLineEdit(parent);
//	sn_ledit_chu1->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_chu1 = new QLabel(parent);
//	pd_label_chu1->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_chu1->setText(QApplication::translate("E2prom Util", "CHU1 Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_chu1 = new QLineEdit(parent);
//	pd_ledit_chu1->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_chu2 = new QLabel(parent);
//	sn_label_chu2->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_chu2->setText(QApplication::translate("E2prom Util", "CHU2 Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_chu2 = new QLineEdit(parent);
//	sn_ledit_chu2->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_chu2 = new QLabel(parent);
//	pd_label_chu2->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_chu2->setText(QApplication::translate("E2prom Util", "CHU2 Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_chu2 = new QLineEdit(parent);
//	pd_ledit_chu2->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_chu3 = new QLabel(parent);
//	sn_label_chu3->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_chu3->setText(QApplication::translate("E2prom Util", "CHU3 Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_chu3 = new QLineEdit(parent);
//	sn_ledit_chu3->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_chu3 = new QLabel(parent);
//	pd_label_chu3->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_chu3->setText(QApplication::translate("E2prom Util", "CHU3 Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_chu3 = new QLineEdit(parent);
//	pd_ledit_chu3->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_chu4 = new QLabel(parent);
//	sn_label_chu4->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_chu4->setText(QApplication::translate("E2prom Util", "CHU4 Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_chu4 = new QLineEdit(parent);
//	sn_ledit_chu4->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_chu4 = new QLabel(parent);
//	pd_label_chu4->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_chu4->setText(QApplication::translate("E2prom Util", "CHU4 Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_chu4 = new QLineEdit(parent);
//	pd_ledit_chu4->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_fanu = new QLabel(parent);
//	sn_label_fanu->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_fanu->setText(QApplication::translate("E2prom Util", "Fan Unit Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_fanu = new QLineEdit(parent);
//	sn_ledit_fanu->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_fanu = new QLabel(parent);
//	pd_label_fanu->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_fanu->setText(QApplication::translate("E2prom Util", "Fan Unit Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_fanu = new QLineEdit(parent);
//	pd_ledit_fanu->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_albox = new QLabel(parent);
//	sn_label_albox->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_albox->setText(QApplication::translate("E2prom Util", "Alarm Box Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_albox = new QLineEdit(parent);
//	sn_ledit_albox->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_albox = new QLabel(parent);
//	pd_label_albox->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_albox->setText(QApplication::translate("E2prom Util", "Alarm Box Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_albox = new QLineEdit(parent);
//	pd_ledit_albox->setGeometry(QRect(x_col[3], y_current, 113, 23));
//}
//
//parameter_panel_bcb::parameter_panel_bcb(QWidget *parent)
//: parameter_panel_general(parent)
//{
//	setup_gui(parent);
//}
//
//void parameter_panel_bcb::setup_gui(QWidget *parent)
//{
//	int x_col[] = {10, 200, 300, 490}, y_gap = 40, y_current = 110;
//
//	sn_label_bcb = new QLabel(parent);
//	sn_label_bcb->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_bcb->setText(QApplication::translate("E2prom Util", "BCB Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_bcb = new QLineEdit(parent);
//	sn_ledit_bcb->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_bcb = new QLabel(parent);
//	pd_label_bcb->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_bcb->setText(QApplication::translate("E2prom Util", "BCB Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_bcb = new QLineEdit(parent);
//	pd_ledit_bcb->setGeometry(QRect(x_col[3], y_current, 113, 23));
//	
//	y_current += y_gap;
//
//	sn_label_lps = new QLabel(parent);
//	sn_label_lps->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_lps->setText(QApplication::translate("E2prom Util", "LPS Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_lps = new QLineEdit(parent);
//	sn_ledit_lps->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_lps = new QLabel(parent);
//	pd_label_lps->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_lps->setText(QApplication::translate("E2prom Util", "LPS Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_lps = new QLineEdit(parent);
//	pd_ledit_lps->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_dpa = new QLabel(parent);
//	sn_label_dpa->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_dpa->setText(QApplication::translate("E2prom Util", "DPA Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_dpa = new QLineEdit(parent);
//	sn_ledit_dpa->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_dpa = new QLabel(parent);
//	pd_label_dpa->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_dpa->setText(QApplication::translate("E2prom Util", "DPA Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_dpa = new QLineEdit(parent);
//	pd_ledit_dpa->setGeometry(QRect(x_col[3], y_current, 113, 23));
//}
//
//parameter_panel_abu::parameter_panel_abu(QWidget *parent)
//: parameter_panel_general(parent)
//{
//	setup_gui(parent);
//}
//
//void parameter_panel_abu::setup_gui(QWidget *parent)
//{
//	int x_col[] = {10, 200, 300, 490}, y_current = 110;
//
//	sn_label_alboard = new QLabel(parent);
//	sn_label_alboard->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_alboard->setText(QApplication::translate("E2prom Util", "Alarm Board Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_alboard = new QLineEdit(parent);
//	sn_ledit_alboard->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_alboard = new QLabel(parent);
//	pd_label_alboard->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_alboard->setText(QApplication::translate("E2prom Util", "Alarm Board Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_alboard = new QLineEdit(parent);
//	pd_ledit_alboard->setGeometry(QRect(x_col[3], y_current, 113, 23));
//}
//
//parameter_panel_bscu::parameter_panel_bscu(QWidget *parent)
//: parameter_panel_general(parent)
//{
//	setup_gui(parent);
//}
//
//void parameter_panel_bscu::setup_gui(QWidget *parent)
//{
//	int x_col[] = {10, 200, 300, 490}, y_gap = 40, y_current = 110;
//
//	sn_label_mb = new QLabel(parent);
//	sn_label_mb->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_mb->setText(QApplication::translate("E2prom Util", "Main Board Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_mb = new QLineEdit(parent);
//	sn_ledit_mb->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_mb = new QLabel(parent);
//	pd_label_mb->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_mb->setText(QApplication::translate("E2prom Util", "Main Board Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_mb = new QLineEdit(parent);
//	pd_ledit_mb->setGeometry(QRect(x_col[3], y_current, 113, 23));
//	
//	y_current += y_gap;
//
//	sn_label_gnss = new QLabel(parent);
//	sn_label_gnss->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_gnss->setText(QApplication::translate("E2prom Util", "GNSS Board Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_gnss = new QLineEdit(parent);
//	sn_ledit_gnss->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_gnss = new QLabel(parent);
//	pd_label_gnss->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_gnss->setText(QApplication::translate("E2prom Util", "GNSS Board Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_gnss = new QLineEdit(parent);
//	pd_ledit_gnss->setGeometry(QRect(x_col[3], y_current, 113, 23));
//
//	y_current += y_gap;
//
//	sn_label_mcb = new QLabel(parent);
//	sn_label_mcb->setGeometry(QRect(x_col[0], y_current, 181, 23));
//	sn_label_mcb->setText(QApplication::translate("E2prom Util", "MCB Serial Number", 0, QApplication::UnicodeUTF8));
//	sn_ledit_mcb = new QLineEdit(parent);
//	sn_ledit_mcb->setGeometry(QRect(x_col[1], y_current, 81, 23));
//
//	pd_label_mcb = new QLabel(parent);
//	pd_label_mcb->setGeometry(QRect(x_col[2], y_current, 181, 23));
//	pd_label_mcb->setText(QApplication::translate("E2prom Util", "MCB Product Date", 0, QApplication::UnicodeUTF8));
//	pd_ledit_mcb = new QLineEdit(parent);
//	pd_ledit_mcb->setGeometry(QRect(x_col[3], y_current, 113, 23));
//}


QT_END_NAMESPACE



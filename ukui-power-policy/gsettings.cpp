/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#include "gsettings.h"

char *get_char_pointer(QString string)
{
    char *buffer;
    buffer = (char *)malloc(string.length() + 1);
    strcpy(buffer, string.toLocal8Bit().data());
    return buffer;
}

QGSettings::QGSettings(QString schema, QObject *parent) : QObject(parent)
{
	gsettings = g_settings_new(schema.toLocal8Bit().data());
	g_signal_connect(gsettings, "changed",
				G_CALLBACK(QGSettings::changedCallback), this);
}

/* value changed callback */
void QGSettings::changedCallback(GSettings *gsettings, const gchar *key,
							gpointer user_data)
{
	(void)gsettings;
	QGSettings *qgsettings = (QGSettings *)user_data;
	Q_EMIT qgsettings->valueChanged(QString::fromLocal8Bit(key));
}

/*
 * Getter
 */

QString QGSettings::getString(QString key)
{
	char *key_str, *value;
	key_str = get_char_pointer(key);
	value = g_settings_get_string(gsettings, key_str);
	free(key_str);
	return QString::fromLocal8Bit(value);
}

int QGSettings::getInt(QString key)
{
	char *key_str;
	int value;
	key_str = get_char_pointer(key);
	value = g_settings_get_int(gsettings, key_str);
	free(key_str);
	return value;
}

bool QGSettings::getBool(QString key)
{
	char *key_str;
	bool value;
	key_str = get_char_pointer(key);
	value = g_settings_get_boolean(gsettings, key_str);
	free(key_str);
	return value;
}

QList<QString> QGSettings::getStringList(QString key)
{
	char *key_str;
	char **value;
	key_str = get_char_pointer(key);
	value = g_settings_get_strv(gsettings, key_str);
	free(key_str);
	QList<QString> list;
	for (; *value; value++)
		list.append(QString::fromLocal8Bit(*value));
	return list;
}

int QGSettings::getEnum(QString key)
{
    char *key_str;
    int value;
    key_str = get_char_pointer(key);
    value = g_settings_get_enum(gsettings, key_str);
    return value;
}


/*
 * Setter
 */

bool QGSettings::setString(QString key, QString value)
{
	char *key_str, *value_str;
	bool ret;
	key_str = get_char_pointer(key);
	value_str = get_char_pointer(value);
	ret = g_settings_set_string(gsettings, key_str, value_str);
	free(key_str);
	free(value_str);
	return ret;
}

bool QGSettings::setInt(QString key, int value)
{
	char *key_str;
	bool ret;
	key_str = get_char_pointer(key);
	ret = g_settings_set_int(gsettings, key_str, value);
	free(key_str);
	return ret;
}

bool QGSettings::setBool(QString key, bool value)
{
	char *key_str;
	bool ret;
	key_str = get_char_pointer(key);
	ret = g_settings_set_boolean(gsettings, key_str, value);
	free(key_str);
	return ret;
}

bool QGSettings::setStringList(QString key, QList<QString> value)
{
	char *key_str;
	const char ** string_array;
	bool ret;
	int i;
	key_str = get_char_pointer(key);
	string_array = (const char **)malloc(value.count() * sizeof(char *));
	for (i = 0; i < value.count(); i++) {
		string_array[i] = value[i].toLocal8Bit().data();
	}
	string_array[i] = 0;
	ret = g_settings_set_strv(gsettings, key_str, string_array);
	free(key_str);
	return ret;
}

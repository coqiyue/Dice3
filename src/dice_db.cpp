#include "dice_db.h"
#include <memory>
#include "SQLiteCpp/SQLiteCpp.h"
#include "cqsdk/cqsdk.h"
#include "dice_exception.h"
#include "dice_msg.h"

namespace dice::db {
    std::unique_ptr<SQLite::Database> db;

    void InitialiseDB() {
        SQLite::Transaction tran(*db);
        db->exec(
            "CREATE TABLE IF NOT EXISTS qq_info (qq_id INTEGER PRIMARY KEY NOT NULL, is_ban INTEGER DEFAULT 0, "
            "ban_reason "
            "TEXT, ban_time INTEGER, "
            "is_admin INTEGER DEFAULT 0, admin_time INTEGER, is_white INTEGER DEFAULT 0, white_time INTEGER, "
            "jrrp_value "
            "INTEGER, jrrp_date "
            "TEXT, bot_on INTEGER DEFAULT 1, card_chosen TEXT DEFAULT \"default\", nick_name TEXT, default_dice "
            "INTEGER NOT NULL DEFAULT 100)");
        db->exec(
            "CREATE TABLE IF NOT EXISTS character_cards (qq_id INTEGER NOT NULL, card_name TEXT NOT NULL, card_detail "
            "TEXT, "
            "PRIMARY KEY (qq_id, card_name))");
        db->exec(
            "CREATE TABLE IF NOT EXISTS group_info (group_id INTEGER NOT NULL, type INTEGER NOT NULL, bot_on INTEGER "
            "DEFAULT 1, help_on INTEGER DEFAULT 1, jrrp_on INTEGER DEFAULT 1, is_ban INTEGER DEFAULT 0, ban_reason "
            "TEXT, ban_time INTEGER, "
            "is_white "
            "INTEGER DEFAULT 0, white_time INTEGER, PRIMARY KEY(group_id, type))");
        db->exec(
            "CREATE TABLE IF NOT EXISTS group_user_info (qq_id INTEGER NOT NULL, group_id INTEGER NOT NULL, type "
            "INTEGER "
            "NOT NULL, nick_name TEXT, card_chosen TEXT DEFAULT \"default\", PRIMARY KEY(qq_id, group_id, type))");
        db->exec("CREATE TABLE IF NOT EXISTS global_msg (title TEXT PRIMARY KEY NOT NULL, val TEXT NOT NULL)");
        db->exec("CREATE TABLE IF NOT EXISTS help_msg (title TEXT PRIMARY KEY NOT NULL, val TEXT NOT NULL)");
        for (const auto& msg : msg::global_msg) {
            SQLite::Statement st(*db, "INSERT OR IGNORE INTO global_msg(title, val) VALUES(?, ?)");
            st.bind(1, msg.first);
            st.bind(2, msg.second);
            st.exec();
        }
        for (const auto& msg : msg::help_msg) {
            SQLite::Statement st(*db, "INSERT OR IGNORE INTO help_msg(title, val) VALUES(?, ?)");
            st.bind(1, msg.first);
            st.bind(2, msg.second);
            st.exec();
        }
        tran.commit();
    }

    void SemiReplaceDB() {
        SQLite::Transaction tran(*db);
        for (const auto& msg : msg::global_msg) {
            SQLite::Statement st(*db, "REPLACE INTO global_msg(title, val) VALUES(?, ?)");
            st.bind(1, msg.first);
            st.bind(2, msg.second);
            st.exec();
        }
        for (const auto& msg : msg::help_msg) {
            SQLite::Statement st(*db, "REPLACE INTO help_msg(title, val) VALUES(?, ?)");
            st.bind(1, msg.first);
            st.bind(2, msg.second);
            st.exec();
        }
        tran.commit();
    }

    void ReplaceDB() {
        db = nullptr;
        if (!std::filesystem::remove(cq::utils::s2ws(cq::api::get_app_directory() + "DiceConfig_"
                                     + std::to_string(cq::api::get_login_user_id()) + ".db"))) {
            cq::logging::debug("Dice! V3", "Unable to Remove database");
        }
        db = std::make_unique<SQLite::Database>(
            cq::api::get_app_directory() + "DiceConfig_" + std::to_string(cq::api::get_login_user_id()) + ".db",
            SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE,
            3000);
        InitialiseDB();
    }

} // namespace dice::db

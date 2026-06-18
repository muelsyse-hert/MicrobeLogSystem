#ifndef STORAGE_H
#define STORAGE_H

#include "common.h"

/* ============================================================================
 * 存储层 - 数据持久化的抽象接口
 * 
 * 该模块为数据存储提供了一个抽象层，允许灵活的后端实现：
 * - 基于文件的存储 (JSON/CSV 格式)
 * - 未来数据库存储 (SQLite、PostgreSQL 等)
 * 
 * 实际应用可以在运行时或编译时选择后端。
 * ============================================================================ */

/* 存储后端类型枚举 */
typedef enum {
    STORAGE_BACKEND_FILE,       /* 基于 JSON/CSV 文件的存储 */
    STORAGE_BACKEND_DATABASE    /* 数据库存储（为未来使用预留）*/
} StorageBackend;

/* 不透明化的存储上下文句柄 */
typedef void StorageContext;

/* ============================================================================
 * 存储后端函数 - 必须由每个后端实现
 * ============================================================================ */

/* 使特定后端初始化存储
 * 参数：
 *   backend - 存储后端类型（文件或数据库）
 *   location - 存储位置（文件路径或数据库连接串）
 * 返回值：
 *   成功时需返回非 NULL，失败时返回 NULL
 */
StorageContext* storage_init(StorageBackend backend, const char* location);

/* 不所有菌株的存储中加载到内存
 * 参数：
 *   context - 存储上下文句柄
 * 返回值：
 *   包含已加载菌株的二叉搜索树的根节点，或者不有数据时返回 NULL
 * 注意：
 *   返回的树是加载到内存中的
 */
StrainNode* storage_load_all_strains(StorageContext* context);

/* 保存单个菌株及其日志到存储
 * 参数：
 *   context - 存储上下文句柄
 *   strain - 指向要保存的菌株节点的指针
 * 返回值：
 *   成功时返回 1，失败时返回 0
 */
int storage_save_strain(StorageContext* context, StrainNode* strain);

/* 保存二叉搜索树中的所有菌株到存储
 * 参数：
 *   context - 存储上下文句柄
 *   root - 二叉搜索树的根节点
 * 返回值：
 *   成功时返回 1，失败时返回 0
 */
int storage_save_all_strains(StorageContext* context, StrainNode* root);

/* 从存储中删除一个菌株
 * 参数：
 *   context - 存储上下文句柄
 *   strain_name - 要删除的菌株名称
 * 返回值：
 *   成功时返回 1，失败时返回 0
 */
int storage_delete_strain(StorageContext* context, const char* strain_name);

/* 检查存储是否有未保存的更改
 * 参数：
 *   context - 存储上下文句柄
 * 返回值：
 *   有未保存更改时返回 1，否则返回 0
 */
int storage_has_changes(StorageContext* context);

/* 标记所有更改为已保存
 * 参数：
 *   context - 存储上下文句柄
 */
void storage_clear_changes(StorageContext* context);

/* 关闭存储并释放资源
 * 参数：
 *   context - 存储上下文句柄
 * 返回值：
 *   成功时返回 1，失败时返回 0
 */
int storage_close(StorageContext* context);

/* ============================================================================
 * 方便函数
 * ============================================================================ */

/* Initialize file storage backend
 * Parameters:
 *   data_dir - Directory to store data files
 * Returns:
 *   Storage context handle, or NULL on failure
 */
StorageContext* storage_file_init(const char* data_dir);

/* Initialize database storage backend (reserved for future implementation)
 * Parameters:
 *   connection_string - Database connection string
 * Returns:
 *   Storage context handle, or NULL on failure (currently returns NULL)
 */
StorageContext* storage_db_init(const char* connection_string);

#endif /* STORAGE_H */

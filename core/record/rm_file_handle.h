#pragma once

#include <assert.h>

#include <memory>

#include "record.h"
#include "txn/txn.h"
#include "txn/batch_txn.h"
#include "util/bitmap.h"
#include "storage/page.h"
#include "storage/disk_manager.h"
#include "buffer/bufferpool_manager.h"

class RmManager;

/* 对表数据文件中的页面进行封装 */
struct RmPageHandle {
    const RmFileHdr *file_hdr;  // 当前页面所在文件的文件头指针
    Page *page;                 // 页面的实际数据，包括页面存储的数据、元信息等
    RmPageHdr *page_hdr;        // page->data的第一部分，存储页面元信息，指针指向首地址，长度为sizeof(RmPageHdr)
    char *bitmap;               // page->data的第二部分，存储页面的bitmap，指针指向首地址，长度为file_hdr->bitmap_size
    char *slots;                // page->data的第三部分，存储表的记录，指针指向首地址，每个slot的长度为file_hdr->record_size

    RmPageHandle(const RmFileHdr *fhdr_, Page *page_) : file_hdr(fhdr_), page(page_) {
        page_hdr = reinterpret_cast<RmPageHdr *>(page->get_data() + OFFSET_PAGE_HDR);
        bitmap = page->get_data() + sizeof(RmPageHdr) + OFFSET_PAGE_HDR;
        slots = bitmap + file_hdr->bitmap_size_;
    }

    int get_slot_offset(int slot_no) const {
        return sizeof(RmPageHdr) + file_hdr->bitmap_size_ + slot_no * (file_hdr->record_size_ + sizeof(itemkey_t));
    }

    int get_slot_no(int slot_offset) const {
        return (slot_offset - sizeof(RmPageHandle) - file_hdr->bitmap_size_) / (file_hdr->record_size_ + sizeof(itemkey_t));
    }

    // 返回指定slot_no的slot存储收地址
    char* get_slot(int slot_offset) const {
        return slots + slot_offset;  // slots的首地址 + slot个数 * 每个slot的大小(每个record的大小)
    }
};

/* 每个RmFileHandle对应一个表的数据文件，里面有多个page，每个page的数据封装在RmPageHandle中 */
class RmFileHandle {      
    // friend class RmScan;    
    // friend class RmManager;

   public:
    RmFileHandle(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager, int fd)
        : disk_manager_(disk_manager), buffer_pool_manager_(buffer_pool_manager), fd_(fd) {
        // 注意：这里从磁盘中读出文件描述符为fd的文件的file_hdr，读到内存中
        // 这里实际就是初始化file_hdr，只不过是从磁盘中读出进行初始化
        // init file_hdr_
        disk_manager_->read_page(fd, PAGE_NO_RM_FILE_HDR, (char *)&file_hdr_, sizeof(file_hdr_));
        // disk_manager管理的fd对应的文件中，设置从file_hdr_.num_pages开始分配page_no
        disk_manager_->set_fd2pageno(fd, file_hdr_.num_pages_);
    }

    // DISALLOW_COPY(RmFileHandle);

    RmFileHdr get_file_hdr() { return file_hdr_; }
    int GetFd() { return fd_; }

    /* 判断指定位置上是否已经存在一条记录，通过Bitmap来判断 */
    bool is_record(const Rid &rid) const {
        RmPageHandle page_handle = fetch_page_handle(rid.page_no_);
        return Bitmap::is_set(page_handle.bitmap, rid.slot_offset_ );  // page的slot_no位置上是否有record
    }

    std::unique_ptr<RmRecord> get_record(const Rid &rid, BatchTxn* txn) const;

    Rid insert_record(itemkey_t key, char *buf, BatchTxn* txn);

    void insert_record(const Rid &rid, char *buf);

    void delete_record(const Rid &rid, BatchTxn* txn);

    void update_record(const Rid &rid, char *buf, BatchTxn* txn);

    RmPageHandle create_new_page_handle(BatchTxn* txn);

    RmPageHandle fetch_page_handle(page_id_t page_no) const;

    DiskManager *disk_manager_;
    BufferPoolManager *buffer_pool_manager_;
    int fd_;        // 打开文件后产生的文件句柄
    RmFileHdr file_hdr_;    // 文件头，维护当前表文件的元数据

   private:
    RmPageHandle create_page_handle(BatchTxn* txn);

    void release_page_handle(RmPageHandle &page_handle);
};
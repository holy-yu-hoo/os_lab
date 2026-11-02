#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/timekeeping.h>
#include <linux/uaccess.h>
#include <linux/version.h>


struct proc_dir_entry* pfile;

static ssize_t procfile_read(struct file* file, char __user* buffer, size_t buffer_length, loff_t* offset) {
    u64 total_seconds = ktime_get_boottime_seconds();
    unsigned int sec = total_seconds % 60;
    unsigned int min = (total_seconds / 60) % 60;
    unsigned int hour = total_seconds / 3600;
    char message[32];
    int length;

    if (*offset != 0)
        return 0;

    length = scnprintf(message, sizeof(message), "%02u:%02u:%02u\n",
        hour, min, sec);
    length = min_t(size_t, length, buffer_length);
    if (copy_to_user(buffer, message, length))
        return -EFAULT;
    *offset += length;
    return length;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_ops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_ops = {
    .read = procfile_read,
};
#endif

static int __init mod_init(void) {
    pr_info("Welcome to the Tomsk State University\n");

    pfile = proc_create("tsulab", 0644, NULL, &proc_file_ops);
    if (!pfile) return -ENOMEM;
    return 0;
}

static void __exit mod_exit(void) {
    proc_remove(pfile);
    pr_info("Tomsk State University forever!\n");
}


module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
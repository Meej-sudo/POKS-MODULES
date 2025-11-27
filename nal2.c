#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t * );
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LENGTH 100

static int Major;
static int readcount=2;
static char msgR[BUF_LENGTH];
static char msgW[BUF_LENGTH];

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

static int __init char_init(void){
    Major = register_chrdev(0,DEVICE_NAME, &fops);
    if(Major<0){
        printk(KERN_ALERT "Registracija neuspešna %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "Dodeljena številka: %d\n", Major);
    printk(KERN_INFO "Za driver potrebuješ dev file\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n",DEVICE_NAME,Major);
    printk(KERN_INFO "Uporabi cat in echo za pisanje in branje\n");
    return SUCCESS;
}
static int device_open(struct inode *inode, struct file *file){
    printk(KERN_INFO "device_open Major = %d\n", Major);
    readcount = 2;
    return SUCCESS;
}



static void __exit char_exit(void){
    printk(KERN_INFO "Goodbye Major=%d\n",Major);
    readcount = 2;
    unregister_chrdev(Major, DEVICE_NAME);
}
static int device_release(struct inode *inode, struct file *file){
    printk(KERN_INFO "Device release Major=%d\n",Major);
    return SUCCESS;
}
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset){
    int bytes_read = 0;
    if(readcount--){
        bytes_read = strlen(msgR);
        printk(KERN_INFO "Device read. %s",msgR);
        copy_to_user(buffer, msgR, bytes_read);
    }
    else
    {
        bytes_read = 0;
        printk(KERN_INFO "device read, bytes_read = 0");
    }
    return bytes_read;
}
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * offset){
    copy_from_user(msgW,buff,len);
    printk(KERN_INFO "device write %s\n",msgW);
    return len;
}
module_init(char_init);
module_exit(char_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MAJ");
MODULE_DESCRIPTION("Demo modul");

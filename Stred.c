#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/semaphore.h>
#define BUFF_SIZE 200
MODULE_LICENSE("Dual BSD/GPL");
DECLARE_WAIT_QUEUE_HEAD(readQ);
DECLARE_WAIT_QUEUE_HEAD(writeQ);

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

struct semaphore sem;
struct fasync_struct *async_queue;

char stred[100];
int pos = 0;
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened stred\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed stred\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
	
{
	int ret;
	char buff[BUFF_SIZE];
//	long int len=0;
	if (endRead){
		pos=0;
		endRead = 0;
		return 0;
	}
	//printk(KERN_INFO "Pre reda\n");
	if(down_interruptible(&sem))
		return -ERESTARTSYS;
	while(strlen(stred) == 0)
	{
		if(wait_event_interruptible(readQ, (strlen(stred)>0)))
		{
			up(&sem);
			if(wait_event_interruptible(readQ,(strlen(stred)>0)))
				return -ERESTARTSYS;
			if(down_interruptible(&sem))
				return -ERESTARTSYS;
		}
	}
	strcpy(buff, stred);
	//printk(KERN_INFO "Kopirao sam\n");
	ret = copy_to_user(buffer, stred, strlen(stred));
	//printk(KERN_INFO "IZBACIO SAM\n");
	if(ret)
	{
		//printk(KERN_WARNING "Nisam izbacio\n");
		return -EFAULT;
	}
	printk(KERN_WARNING "Succesfully read\n");
	endRead=1;



//		for(pos=0; pos<100; pos++)
//		{
//			len = scnprintf(buff, BUFF_SIZE, "%c ", stred[pos]);
//			ret = copy_to_user(buffer, buff,len);
//			if(ret)
//				return -EFAULT;
//		}
//		if(pos==100)
//		{
//			printk(KERN_INFO "Succesfully read\n");
//			endRead = 1;
		
//		}
	//else
	//{
	//		printk(KERN_WARNING "Lifo is empty\n"); 
	//}

	return strlen(stred);
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	char *pok;
	int value;
	int ret;
	int j=0;
	char *poklapanje;

	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';
	
		
	if(length<110)
        {
		if(strncmp(buff, "string=", 7)==0)
		{
			if(down_interruptible(&sem))
				return -ERESTARTSYS;
			while(strlen(buff+7) == 100)
			{
				up(&sem);
				if(wait_event_interruptible(writeQ,(strlen(buff+7)<100)))
					return -ERESTARTSYS;
				if(down_interruptible(&sem))
					return -ERESTARTSYS;
			}

			strcpy(stred, (buff+7));
			printk(KERN_WARNING "Succesfully wrote string\n");
			up(&sem);
			wake_up_interruptible(&readQ);
		}
		else if(strcmp(buff,"clear")==0)
		{
			printk(KERN_WARNING "Deleting string\n");
			for(j=0; j<100; j++)
				stred[j]=0;
			up(&sem);
			wake_up_interruptible(&writeQ);
		}
		else if(strcmp(buff, "shrink")==0)
		{
			pok = strim(stred);
			printk(KERN_WARNING "Deleting spaces\n");
			j=0;
			while(stred[j] != '\0')
			{
				stred[j]=(*(pok+j));
				j++;
			}
			stred[j]='\0';
			up(&sem);
			wake_up_interruptible(&writeQ);

		}
		else if(strncmp(buff, "append=", 7)==0)
		{
			if(down_interruptible(&sem))
				return -ERESTARTSYS;
			while(strlen(stred) == 100)
			{
				up(&sem);
				if(wait_event_interruptible(writeQ, (strlen(stred)<100)))
					return -ERESTARTSYS;
				if(down_interruptible(&sem))
					return -ERESTARTSYS;
			}

			j=0;
			printk(KERN_WARNING "Adding string\n");
			//while(buff[j]!='\0')
			//{
		//		if((strlen(stred)+strlen(buff+7))>100)
		//		printk(KERN_WARNING "Too long string\n");
		//		else
		//		{
					strcat(stred, buff+7);
					printk(KERN_INFO "Successfully concatenated\n");
		//		}
			up(&sem);
			wake_up_interruptible(&readQ);

			//}
		}
		else if(strncmp(buff,"truncate=", 9)==0)
		{
			//printk(KERN_WARNING "%s", buff);
			ret = sscanf(buff+9,"%d",&value);
			//printk(KERN_WARNING "%d %d", ret, value);
			if(ret==1)//one parameter parsed in sscanf
			{
				if(value>100 || (strlen(stred)-value)<0)
					printk(KERN_INFO "Can't delete soo many characters\n");
				else
				{
					printk(KERN_INFO "Deleting %d end characters", value); 
					stred[strlen(stred)-value]='\0';	
				}

			}
			else
			{
				printk(KERN_WARNING "Wrong command format\n");
				
			}
			up(&sem);
			wake_up_interruptible(&writeQ);


		}
		else if(strncmp(buff,"remove=", 7)==0)
		{
			memmove(buff, buff+7, strlen(buff)-6);
			while((poklapanje=strstr(stred, buff))!='\0')
			{
				*poklapanje='\0';
				strcat(stred, poklapanje+strlen(buff));
			}
			printk(KERN_INFO "Successfully removed substring");
		}
		else
			printk(KERN_WARNING "Wrong command format\n");
		up(&sem);
		wake_up_interruptible(&writeQ);


	}
	else
		printk(KERN_WARNING "Too long string. The string should not have more than 100 characters");


	return length;
}

static int __init stred_init(void)
{
   int ret = 0;
	int i=0;

	//Initialize array
	for (i=0; i<100; i++)
		stred[i] = 0;
	sema_init(&sem,1);

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);

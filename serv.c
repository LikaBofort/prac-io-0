#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/time.h>

int R = 0;

enum
{
	length = 128
};

struct client
{
	int fd;
	char data[length];
	struct client *next;
};

int str_len (char str[length])
{
	int i;

	for (i = 0; i < length ; i++)
		if ((str[i] == '\0') || (str[i] == '\r'))
			return i;
	return -1;
}

char *get_str_fd (struct client *cl, int fd)
{
	char *str = NULL;

	while (cl)
	{
		if (cl->fd == fd)
			str = cl->data;
		cl = cl->next;
	}
	return str;
}

int found_cmd (struct client *cl, int fd)
{
 	int i;
	char *cmd = get_str_fd(cl, fd);

	for (i = 0; i <= str_len(cmd); i++)
		if (cmd[i] == '\r')
			return 1;
	return 0;
}

void bind_err (int port)
{
	if (port < 1025)
		printf("Error! Attempt to use privileged port\n");
	else
		printf("Error! Attempt to use employed port\n ");
}

int len_client (struct client * cl)
{
	int i = 0;

	while (cl)
	{
		i++;
		cl = cl->next;
	}
	return i;
}

int curr_cl (struct client *cl)
{
	int n = 0;

	while (cl)
	{
		n++;
		cl = cl->next;
	}
	return n;
}

int get_fd_num (struct client *cl, int n)
{
	int i = 0;

	while (cl)
	{
		i++;
		if (i == n)
			return cl->fd;
		cl = cl->next;
	}
	return -1;
}

int get_num_fd (struct client *cl, int fd)
{
	int i = 0;

	while (cl)
	{
		i++;
		if (cl->fd == fd)
			return i;
		cl = cl->next;
	}

	return -1;
}

struct client *new_cl (struct client *cl, int fd)
{
	struct client *tmp = malloc(sizeof(struct client));

	if (fd == -1)
	{
		perror ("accept");
		return NULL;
	}
	tmp->fd = fd;
	tmp->data[0] = '\0';
	tmp->next = NULL;

	if (!cl)
		cl = tmp;
	else
	{
		struct client *curr;
		curr = cl;
		while (curr->next)
			curr = curr->next;
		curr->next = tmp;
	}

	return cl;
}

struct client *add_data(struct client *cl, int n, char str[length])
{
	int i, f;
	struct client *new_cl;
	char err[length] = "Long command!\n";

	new_cl = cl;

	for (i = 1; i < n; i++)
		cl = cl->next;
	f = str_len(cl->data);
	if (str_len(str)+f > length/2)
	{
		write (cl->fd, err, str_len(err)+1);
		cl->data[0] = '\0';
	}
	else
		for (i = f; i < str_len(str)+1+f; i++)
			cl->data[i] = str [i-f];

	return new_cl;
}

struct client *delete_cl (struct client *cl, int fd)
{
	int n = get_num_fd(cl, fd), i;
	struct client *prev, *curr;

	prev = cl;
	curr = cl;

	if (n == -1)
		return NULL;

	if (n == 1)
	{
		cl = cl->next;
		free(curr);
		return cl;
	}
	for (i = 1; i < n-1; i++)
		prev = prev->next;
	for (i = 1; i < n+1; i++)
		curr = curr->next;
	free(prev->next);
	prev->next = curr;
	return cl;
}

struct client *reading (struct client *cl, int fd)
{
	int n = get_num_fd(cl, fd), numb;
	char str[length];
	struct client *new_cl;

	new_cl = cl;

	if (n == -1)
		return NULL;
	numb = read(fd, str, sizeof(str)-1);
	if (numb == -1)
	{
		perror("read");
		return NULL;
	}
	if (numb == 0)
	{
		int len;
		printf ("Client with fd = %d disconnected\n",fd);
		shutdown(fd,2);
		close(fd);
		len = curr_cl(cl);
		if (len > 1)
		{
			cl = delete_cl(cl, fd);
			return cl;
		}
		else
		{
			free(cl);
			return NULL;
		}
	}
	str[numb] = '\0';
	cl = add_data(cl, n, str);

	return new_cl;
}

int eq_str (char *str1, char *str2)
{
	int i;

	if (str_len(str1) != str_len(str2))
		return 0;
	for (i = 0; i < str_len(str1); i++)
		if (str1[i] != str2[i])
			return 0;
	return 1;
}

void print_R (int fd)
{
	char str[length];

	sprintf(str, "R = %d\n", R);
	write(fd, str, str_len(str)+1);
}

void print_arg (int fd, int curr_numb)
{
	char str[length];

	sprintf(str, "%d clients\n", curr_numb);
	write(fd, str, str_len(str)+1);
	sprintf(str, "R = %d\n", R);
	write(fd, str, str_len(str)+1);
}

void wr (int fd, char str[length])
{
	write (fd, str, str_len(str)+1);
}

void print_help (int fd)
{
	char str1[length] = "\nprint - show value of R\n";
	char str2[length] = "  arg - show client's number and value of R\n";
	char str3[length] = "   up - R++;\n";
	char str4[length] = " down - R--;\n";
	char str5[length] = " help - commands which you can use\n\n";

	wr(fd,str1);
	wr(fd,str2);
	wr(fd,str3);
	wr(fd,str4);
	wr(fd,str5);

}

struct client *processing (struct client *cl, int fd, int curr_numb)
{
	int i, k = get_num_fd(cl, fd);
	struct client *new_cl;
	char str[length] = "Wrong command! Try 'help'\n";

	new_cl = cl;
	if (k == -1)
		return cl;
	if (found_cmd(cl, fd))
	{
		for (i = 1; i < k; i++)
			cl = cl->next;
		if (eq_str("up\0", cl->data))
			R++;
		else if (eq_str("down\0", cl->data))
			R--;
		else if (eq_str("print\0", cl->data))
			print_R (fd);
		else if (eq_str("arg\0", cl->data))
			print_arg (fd, curr_numb);
		else if (eq_str("help\0", cl->data))
			print_help (fd);
		else
			write (fd, str, str_len(str)+1);

		cl->data[0] = '\0';
		return new_cl;
	}
	return cl;
}
struct client *closing (int curr_fd, struct client *cl)
{
	shutdown(curr_fd, 2);
	close(curr_fd);
	cl = delete_cl(cl, curr_fd);
	return cl;
}

int do_it (int ls, int numb)
{
	struct client *cl = NULL;
	int curr_numb = 0;
	char err_numb[length] = "Error! Wrong number of clients\n";
	for (;;)
	{
		fd_set readfds;
		int max_d = ls, fd, i;
		FD_ZERO(&readfds);
		FD_SET(ls, &readfds);
		curr_numb = curr_cl(cl);
		for (i = 0; i < curr_numb; i++)
		{
			fd = get_fd_num(cl, i+1);
			FD_SET(fd, &readfds);
			if (fd > max_d)
				max_d = fd;
		}
		int res = select(max_d+1,&readfds,NULL,NULL,NULL);
		if (res < 1)
			perror ("select");
		if (FD_ISSET(ls, &readfds))
		{
			int curr_fd;
			if (-1 == (curr_fd = accept(ls, NULL, NULL)))
				perror("accept");
			cl = new_cl(cl, curr_fd);
			curr_numb = curr_cl(cl);
			if (curr_numb > numb)
			{
				write (curr_fd, err_numb, str_len(err_numb)+1);
				cl = closing(curr_fd, cl);
				curr_numb = curr_cl(cl);
				continue;
			}
			printf("Client with fd = %d connected\n",curr_fd);
		}
		for (i = 0; i < curr_numb; i++)
		{
			fd = get_fd_num(cl, i+1);
			if (FD_ISSET(fd, &readfds))
			{
				cl = reading(cl, fd);
				curr_numb = curr_cl(cl);
				if (!cl)
					break;
				cl = processing(cl, fd, curr_numb);
			}
		}
	}
	return 0;
}

int main (int argc, char **argv)
{
	struct sockaddr_in addr;
	int ls, numb, port, qlen = 5, opt = 1;

	sscanf(argv[1], "%d", &numb);
	sscanf(argv[2], "%d", &port);

	if (port > 0)
	{
		if (-1 == (ls = socket(AF_INET, SOCK_STREAM, 0)))
		{
			printf("Error in socket's creating\n");
			return 1;
		}
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;
		setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if (0 != bind(ls, (struct sockaddr *)&addr, sizeof(addr)))
			bind_err(port);
		else if (-1 == listen(ls, qlen))
			perror ("listen");
		else
			do_it(ls, numb);
		}

	return 0;
}

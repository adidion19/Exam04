/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   last_one.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adidion <adidion@student.s19.be>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/24 10:02:42 by adidion           #+#    #+#             */
/*   Updated: 2022/02/24 13:31:40 by adidion          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>

int	ft_strlen(char *str)
{
	if (!str)
		return (0);
	int i = -1;
	while (str[++i]);
	return (i);
}

char *ft_strdup(char *str)
{
	if (!str)
		return (NULL);
	char *str2 = (char *)malloc(sizeof(char) * (ft_strlen(str) + 1));
	int i = -1;
	while (str[++i])
	{
		str2[i] = str[i];
	}
	str2[i] = '\0';
	return (str2);
}

void	ft_putstr(char *str)
{
	write(2, str, ft_strlen(str));
}

char **ft_parse(int ac, char **av, int *index, int *bool)
{
	int i = *index;
	if (!av || !av[i])
		return (NULL);
	char **cmd = (char **)malloc(sizeof(char *) * (ac + 1));
	if (!cmd)
	{
		exit(EXIT_FAILURE);
	}
	int j = 0;
	while (av[i][0] == '|' || av[i][0] == ';')
	{
		i++;}
	while (av[i] && av[i][0] != '|' && av[i][0] != ';')
	{
		cmd[j] = ft_strdup(av[i]);
		i++;
		j++;
	}
	*bool = 0;
	cmd[j] = NULL;
	if (!av[i])
	{
		*bool = 1;
		*index = i;
		return (cmd);
	}
	if (av[i][0] == ';')
	{
		*bool = 1;
		while (av[i] && av[i][0] != ';')
		{
			i++;
		}
	}
	*index = i+1;
	return (cmd);
}

void ft_cd(char **cmd)
{
	int i = -1;
	while (cmd[++i]);
	if (i != 1)
	{
		ft_putstr("cd: bad arguments\n");
		return ;
	}
	if (-1 == chdir(cmd[1]))
	{
		ft_putstr("can't acces to path ");
		ft_putstr(cmd[1]);
		ft_putstr("\n");
	}
	return ;
}

void	ft_free(char **cmd)
{
	int i = -1;
	if (!cmd)
		free(cmd);
	if (!cmd)
		return ;
	while (cmd[++i])
	{
		free(cmd[i]);
	}
	free(cmd[i]);
	free(cmd);
	return ;
}

void	ft_pipeline(int ac, char **av, char **env)
{
	int bool = 0;
	int i = 1;
	char **cmd;
	int pid;
	int fd[2];
	while ((cmd = ft_parse(ac, av, &i, &bool)))
	{
		if (!cmd[0])
		{
			break;
		}
		if (!strcmp(cmd[0], "cd"))
		{
			ft_cd(cmd);
			if (bool != 1)
			ft_free(cmd);
			continue ;
		}
		pipe(fd);
		pid = fork();
		if (pid == -1)
			exit (EXIT_FAILURE);
		if (pid == 0)
		{
			if (bool != 1)
			{
				dup2(fd[1], 1);
				close (fd[0]);
				close (fd[1]);
			}
			execve(cmd[0], cmd, env);
			ft_putstr("Error: can't execute the command ");
			ft_putstr(cmd[0]);
			ft_putstr("\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			waitpid(0,0,0);
			if (bool != 1)
			{
				dup2(fd[0], 0);
				close (fd[0]);
				close (fd[1]);
			}
		}
			ft_free(cmd);
	}
	ft_free(cmd);
	return ;
}

int main(int ac, char **av, char **env)
{
	if (ac >= 2)
		ft_pipeline(ac, av, env);
	//system("leaks a.out");
	return (0);
}
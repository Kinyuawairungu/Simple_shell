#include "main.h"

/**
 * show_env - display env
 * @data: struct for the program's data
 * Return: 0
 */
int show_env(data_of_program *data)
{
	int i;
	char cpname[50] = {'\0'};
	char *var_copy = NULL;

	if (data->tokens[1] == NULL)
		show_environ(data);
	else
	{
		for (i = 0; data->tokens[1][i]; i++)
		{
			if (data->tokens[1][i] == '=')
			{
				var_copy = str_dup(get_env_var(cpname, data));
				if (var_copy != NULL)
					env_set_key(cpname, data->tokens[1] + i + 1, data);

				show_environ(data);
				if (get_env_var(cpname, data) == NULL)
				{
					_print(data->tokens[1]);
					_print("\n");
				}
				else
				{
					env_set_key(cpname, var_copy, data);
					free(var_copy);
				}
				return (0);
			}
			cpname[i] = data->tokens[1][i];
		}
		errno = 2;
		perror(data->command_name);
		errno = 127;
	}
	return (0);
}

/**
 * create_env - set up env
 * @data: program's data
 * Return: 0
 */
int create_env(data_of_program *data)
{
	if (data->tokens[1] == NULL || data->tokens[2] == NULL)
		return (0);
	if (data->tokens[3] != NULL)
	{
		errno = E2BIG;
		perror(data->command_name);
		return (5);
	}

	env_set_key(data->tokens[1], data->tokens[2], data);

	return (0);
}

/**
 * destroy_env - unset env
 * @data: program's data'
 * Return: 0
 */
int destroy_env(data_of_program *data)
{
	if (data->tokens[1] == NULL)
		return (0);
	if (data->tokens[2] != NULL)
	{
		errno = E2BIG;
		perror(data->command_name);
		return (5);
	}
	env_delete_key(data->tokens[1], data);

	return (0);
}

/**
 * builtins1 - functions to find matches and run
 * @data: struct for the program's data
 * Return: func exceecuted or -1
 */
int builtins1(data_of_program *data)
{
	int iterator;
	builtins options[] = {
		{"exit", myexit},
		{"help", myhelp},
		{"cd", mycd},
		{"alias", another_alias},
		{"env", show_env},
		{"setenv", create_env},
		{"unsetenv", destroy_env},
		{NULL, NULL}
	};

	for (iterator = 0; options[iterator].builtin != NULL; iterator++)
	{
		if (str_compare(options[iterator].builtin, data->command_name, 0))
		{
			return (options[iterator].function(data));
		}
	}
	return (-1);
}


/**
 * myexit - function to close prog
 * @data: struct for the program's data
 * Return: 0 or error
 */
int myexit(data_of_program *data)
{
	int i;

	if (data->tokens[1] != NULL)
	{
		for (i = 0; data->tokens[1][i]; i++)
			if ((data->tokens[1][i] < '0' || data->tokens[1][i] > '9')
				&& data->tokens[1][i] != '+')
			{
				errno = 2;
				return (2);
			}
		errno = _atoi(data->tokens[1]);
	}
	free_data(data);
	exit(errno);
}

/**
 * mycd - change dir
 * @data: struct for the program's data
 * Return: 0 or error
 */
int mycd(data_of_program *data)
{
	char *dir_home = get_env_var("HOME", data), *dir_old = NULL;
	char old_dir[128] = {0};
	int error_code = 0;

	if (data->tokens[1])
	{
		if (str_compare(data->tokens[1], "-", 0))
		{
			dir_old = get_env_var("OLDPWD", data);
			if (dir_old)
				error_code = mymkdir(data, dir_old);
			_print(get_env_var("PWD", data));
			_print("\n");

			return (error_code);
		}
		else
		{
			return (mymkdir(data, data->tokens[1]));
		}
	}
	else
	{
		if (!dir_home)
			dir_home = getcwd(old_dir, 128);

		return (mymkdir(data, dir_home));
	}
	return (0);
}

/**
 * mymkdir - create dir
 * @data: struct for the program's data
 * @new_dir: implementation
 * Return: 0 or error
 */
int mymkdir(data_of_program *data, char *new_dir)
{
	char old_dir[128] = {0};
	int err_code = 0;

	getcwd(old_dir, 128);

	if (!str_compare(old_dir, new_dir, 0))
	{
		err_code = chdir(new_dir);
		if (err_code == -1)
		{
			errno = 2;
			return (3);
		}
		env_set_key("PWD", new_dir, data);
	}
	env_set_key("OLDPWD", old_dir, data);
	return (0);
}

/**
 * myhelp - display env
 * @data: struct for the program's data
 * Return: 0 or error
 */
int myhelp(data_of_program *data)
{
	int i, length = 0;
	char *mensajes[6] = {NULL};

	mensajes[0] = HELP_MSG;

	if (data->tokens[1] == NULL)
	{
		_print(mensajes[0] + 6);
		return (1);
	}
	if (data->tokens[2] != NULL)
	{
		errno = E2BIG;
		perror(data->command_name);
		return (5);
	}
	mensajes[1] = HELP_EXIT_MSG;
	mensajes[2] = HELP_ENV_MSG;
	mensajes[3] = HELP_SETENV_MSG;
	mensajes[4] = HELP_UNSETENV_MSG;
	mensajes[5] = HELP_CD_MSG;

	for (i = 0; mensajes[i]; i++)
	{
		length = str_len(data->tokens[1]);
		if (str_compare(data->tokens[1], mensajes[i], length))
		{
			_print(mensajes[i] + length + 1);
			return (1);
		}
	}
	errno = EINVAL;
	perror(data->command_name);
	return (0);
}

/**
 * another_alias - handle alias
 * @data: struct for the program's data
 * Return: 0
 */
int another_alias(data_of_program *data)
{
	int i = 0;

	if (data->tokens[1] == NULL)
		return (display_alias(data, NULL));

	while (data->tokens[++i])
	{
		if (count_char(data->tokens[i], "="))
			put_alias(data->tokens[i], data);
		else
			display_alias(data, data->tokens[i]);
	}

	return (0);
}

#include "main.h"
/**
 * _strtok - split str
 * @line: command
 * @delimator: marker
 * Return: result
*/
char *_strtok(char *line, char *delimator)
{
	int j;
	static char *str;
	char *copystr;

	if (line != NULL)
		str = line;
	for (; *str != '\0'; str++)
	{
		for (j = 0; delimator[j] != '\0'; j++)
		{
			if (*str == delimator[j])
			break;
		}
		if (delimator[j] == '\0')
			break;
	}
	copystr = str;
	if (*copystr == '\0')
		return (NULL);
	for (; *str != '\0'; str++)
	{
		for (j = 0; delimator[j] != '\0'; j++)
		{
			if (*str == delimator[j])
			{
				*str = '\0';
				str++;
				return (copystr);
			}
		}
	}
	return (copystr);
}

/**
 * split_str - ..
 * @data: the program's data
 * Return: different commands
 */
void split_str(data_of_program *data)
{
	char *delimiter = " \t";
	int i, j, counter = 2, length;

	length = str_len(data->input_line);
	if (length)
	{
		if (data->input_line[length - 1] == '\n')
			data->input_line[length - 1] = '\0';
	}

	for (i = 0; data->input_line[i]; i++)
	{
		for (j = 0; delimiter[j]; j++)
		{
			if (data->input_line[i] == delimiter[j])
				counter++;
		}
	}

	data->tokens = malloc(counter * sizeof(char *));
	if (data->tokens == NULL)
	{
		perror(data->program_name);
		exit(errno);
	}
	i = 0;
	data->tokens[i] = str_dup(_strtok(data->input_line, delimiter));
	data->command_name = str_dup(data->tokens[0]);
	while (data->tokens[i++])
	{
		data->tokens[i] = str_dup(_strtok(NULL, delimiter));
	}
}


/**
 * expand_variables - ..
 * @data: pointer to the program's data
 */
void expand_variables(data_of_program *data)
{
	int i, j;
	char line[BUFFER_SIZE] = {0}, expansion[BUFFER_SIZE] = {'\0'}, *temp;

	if (data->input_line == NULL)
		return;
	append_str(line, data->input_line);
	for (i = 0; line[i]; i++)
		if (line[i] == '#')
			line[i--] = '\0';
		else if (line[i] == '$' && line[i + 1] == '?')
		{
			line[i] = '\0';
			num_to_str(errno, expansion, 10);
			append_str(line, expansion);
			append_str(line, data->input_line + i + 2);
		}
		else if (line[i] == '$' && line[i + 1] == '$')
		{
			line[i] = '\0';
			num_to_str(getpid(), expansion, 10);
			append_str(line, expansion);
			append_str(line, data->input_line + i + 2);
		}
		else if (line[i] == '$' && (line[i + 1] == ' ' || line[i + 1] == '\0'))
			continue;
		else if (line[i] == '$')
		{
			for (j = 1; line[i + j] && line[i + j] != ' '; j++)
				expansion[j - 1] = line[i + j];
			temp = get_env_var(expansion, data);
			line[i] = '\0', expansion[0] = '\0';
			append_str(expansion, line + i + j);
			temp ? append_str(line, temp) : 1;
			append_str(line, expansion);
		}
	if (!str_compare(data->input_line, line, 0))
	{
		free(data->input_line);
		data->input_line = str_dup(line);
	}
}

/**
 * more_alias - ..
 * @data: program's data
 */
void more_alias(data_of_program *data)
{
	int i, j, was_expanded = 0;
	char line[BUFFER_SIZE] = {0}, expansion[BUFFER_SIZE] = {'\0'}, *temp;

	if (data->input_line == NULL)
		return;

	append_str(line, data->input_line);

	for (i = 0; line[i]; i++)
	{
		for (j = 0; line[i + j] && line[i + j] != ' '; j++)
			expansion[j] = line[i + j];
		expansion[j] = '\0';

		temp = retrieve_alias(data, expansion);
		if (temp)
		{
			expansion[0] = '\0';
			append_str(expansion, line + i + j);
			line[i] = '\0';
			append_str(line, temp);
			line[str_len(line)] = '\0';
			append_str(line, expansion);
			was_expanded = 1;
		}
		break;
	}
	if (was_expanded)
	{
		free(data->input_line);
		data->input_line = str_dup(line);
	}
}

/**
 * append_str - append string at end of the buffer
 * @buffer: buffer
 * @str: value
 * Return: 0
 */
int append_str(char *buffer, char *str)
{
	int length, i;

	length = str_len(buffer);
	for (i = 0; str[i]; i++)
	{
		buffer[length + i] = str[i];
	}
	buffer[length + i] = '\0';
	return (length + i);
}


/**
 * display_alias - function to handle aliases
 * @data: program's data
 * @alias: name of the alias to be printed
 * Return: 0
 */
int display_alias(data_of_program *data, char *alias)
{
	int i, j, alias_length;
	char buffer[250] = {'\0'};

	if (data->alias_list)
	{
		alias_length = str_len(alias);
		for (i = 0; data->alias_list[i]; i++)
		{
			if (!alias || (str_compare(data->alias_list[i], alias, alias_length)
				&&	data->alias_list[i][alias_length] == '='))
			{
				for (j = 0; data->alias_list[i][j]; j++)
				{
					buffer[j] = data->alias_list[i][j];
					if (data->alias_list[i][j] == '=')
						break;
				}
				buffer[j + 1] = '\0';
				append_str(buffer, "'");
				append_str(buffer, data->alias_list[i] + j + 1);
				append_str(buffer, "'\n");
				_print(buffer);
			}
		}
	}

	return (0);
}

/**
 * retrieve_alias - handle aliases
 * @data: struct for the program's data
 * @name: name of the requested alias.
 * Return: 0
 */
char *retrieve_alias(data_of_program *data, char *name)
{
	int i, alias_length;

	if (name == NULL || data->alias_list == NULL)
		return (NULL);

	alias_length = str_len(name);

	for (i = 0; data->alias_list[i]; i++)
	{
		if (str_compare(name, data->alias_list[i], alias_length) &&
			data->alias_list[i][alias_length] == '=')
		{
			return (data->alias_list[i] + alias_length + 1);
		}
	}
	return (NULL);

}

/**
 * put_alias - implement alias
 * @alias_string: value
 * @data: struct for the program's data
 * Return: 0
 */
int put_alias(char *alias_string, data_of_program *data)
{
	int i, j;
	char buffer[250] = {'0'}, *temp = NULL;

	if (alias_string == NULL ||  data->alias_list == NULL)
		return (1);
	for (i = 0; alias_string[i]; i++)
		if (alias_string[i] != '=')
			buffer[i] = alias_string[i];
		else
		{
			temp = retrieve_alias(data, alias_string + i + 1);
			break;
		}

	for (j = 0; data->alias_list[j]; j++)
		if (str_compare(buffer, data->alias_list[j], i) &&
			data->alias_list[j][i] == '=')
		{
			free(data->alias_list[j]);
			break;
		}

	if (temp)
	{
		append_str(buffer, "=");
		append_str(buffer, temp);
		data->alias_list[j] = str_dup(buffer);
	}
	else
		data->alias_list[j] = str_dup(alias_string);
	return (0);
}

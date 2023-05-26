#include "main.h"

/**
 * _print - is the function to output chars
 * @str: will test par
 * Return: will count int
 */
int _print(char *str)
{
	return (write(STDOUT_FILENO, str, str_len(str)));
}
/**
 * _printe - is the stderr
 * @str: will test par
 * Return: will count output
 */
int _printe(char *str)
{
	return (write(STDERR_FILENO, str, str_len(str)));
}

/**
 * _print_error - ..(error to be displayed)
 * @data: is the program's data'
 * @errorcode: is the error code
 * Return: will count ouput
 */
int _print_error(int errorcode, data_of_program *data)
{
	char n_as_string[10] = {'\0'};

	num_to_str((long) data->exec_counter, n_as_string, 10);

	if (errorcode == 2 || errorcode == 3)
	{
		_printe(data->program_name);
		_printe(": ");
		_printe(n_as_string);
		_printe(": ");
		_printe(data->tokens[0]);
		if (errorcode == 2)
			_printe(": Illegal number: ");
		else
			_printe(": can't cd to ");
		_printe(data->tokens[1]);
		_printe("\n");
	}
	else if (errorcode == 127)
	{
		_printe(data->program_name);
		_printe(": ");
		_printe(n_as_string);
		_printe(": ");
		_printe(data->command_name);
		_printe(": not found\n");
	}
	else if (errorcode == 126)
	{
		_printe(data->program_name);
		_printe(": ");
		_printe(n_as_string);
		_printe(": ");
		_printe(data->command_name);
		_printe(": Permission denied\n");
	}
	return (0);
}

/**
* mygetline - is the function to read input
* @data: is the program's data
* Return: will count output
*/
int mygetline(data_of_program *data)
{
	char buff[BUFFER_SIZE] = {'\0'};
	static char *array_commands[10] = {NULL};
	static char array_operators[10] = {'\0'};
	ssize_t bytes_read, i = 0;

	if (!array_commands[0] || (array_operators[0] == '&' && errno != 0) ||
		(array_operators[0] == '|' && errno == 0))
	{
		for (i = 0; array_commands[i]; i++)
		{
			free(array_commands[i]);
			array_commands[i] = NULL;
		}

		bytes_read = read(data->file_descriptor, &buff, BUFFER_SIZE - 1);
		if (bytes_read == 0)
			return (-1);

		i = 0;
		do {
			array_commands[i] = str_dup(_strtok(i ? NULL : buff, "\n;"));
			i = check_logic_ops(array_commands, i, array_operators);
		} while (array_commands[i++]);
	}
	data->input_line = array_commands[0];
	for (i = 0; array_commands[i]; i++)
	{
		array_commands[i] = array_commands[i + 1];
		array_operators[i] = array_operators[i + 1];
	}

	return (str_len(data->input_line));
}


/**
* check_logic_ops - will handle && and || operators
* @array_commands: will be the commands.
* @i: is the index
* @array_operators: is the logical operators
* Return: will return index of the last command
*/
int check_logic_ops(char *array_commands[], int i, char array_operators[])
{
	char *temp = NULL;
	int j;

	for (j = 0; array_commands[i] != NULL  && array_commands[i][j]; j++)
	{
		if (array_commands[i][j] == '&' && array_commands[i][j + 1] == '&')
		{

			temp = array_commands[i];
			array_commands[i][j] = '\0';
			array_commands[i] = str_dup(array_commands[i]);
			array_commands[i + 1] = str_dup(temp + j + 2);
			i++;
			array_operators[i] = '&';
			free(temp);
			j = 0;
		}
		if (array_commands[i][j] == '|' && array_commands[i][j + 1] == '|')
		{

			temp = array_commands[i];
			array_commands[i][j] = '\0';
			array_commands[i] = str_dup(array_commands[i]);
			array_commands[i + 1] = str_dup(temp + j + 2);
			i++;
			array_operators[i] = '|';
			free(temp);
			j = 0;
		}
	}
	return (i);
}

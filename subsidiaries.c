#include "main.h"

/**
 * free_recurrent_data - function to keep the buffers clean
 * @data: ..
 */
void free_recurrent_data(data_of_program *data)
{
	if (data->tokens)
		free_pointers_array(data->tokens);
	if (data->input_line)
		free(data->input_line);
	if (data->command_name)
		free(data->command_name);

	data->input_line = NULL;
	data->command_name = NULL;
	data->tokens = NULL;
}

/**
 * free_data - clean all data
 * @data: ..
 */
void free_data(data_of_program *data)
{
	if (data->file_descriptor != 0)
	{
		if (close(data->file_descriptor))
			perror(data->program_name);
	}
	free_recurrent_data(data);
	free_pointers_array(data->env);
	free_pointers_array(data->alias_list);
}

/**
 * free_pointers_array - ..
 * @myarray: array of pointers
 */
void free_pointers_array(char **myarray)
{
	int i;

	if (myarray != NULL)
	{
		for (i = 0; myarray[i]; i++)
			free(myarray[i]);

		free(myarray);
		myarray = NULL;
	}
}

/**
 * num_to_str - change num to str
 * @num: test par
 * @str: output
 * @base: type
 */
void num_to_str(long num, char *str, int base)
{
	int index = 0, inNegative = 0;
	long cociente = num;
	char letters[] = {"0123456789abcdef"};

	if (cociente == 0)
		str[index++] = '0';

	if (str[0] == '-')
		inNegative = 1;

	while (cociente)
	{
		if (cociente < 0)
			str[index++] = letters[-(cociente % base)];
		else
			str[index++] = letters[cociente % base];
		cociente /= base;
	}
	if (inNegative)
		str[index++] = '-';

	str[index] = '\0';
	str_rev(str);
}


/**
 * _atoi - change str to num
 * @str: test par
 * Return: num
 */
int _atoi(char *str)
{
	int sign = 1;
	unsigned int number = 0;

	while (!('0' <= *str && *str <= '9') && *str != '\0')
	{
		if (*str == '-')
			sign *= -1;
		if (*str == '+')
			sign *= +1;
		str++;
	}

	while ('0' <= *str && *str <= '9' && *str != '\0')
	{

		number = (number * 10) + (*str - '0');
		str++;
	}
	return (number * sign);
}

/**
 * count_char - count repetition
 * @str: test par
 * @chars: string with  chars
 * Return: int
 */
int count_char(char *str, char *chars)
{
	int i = 0, counter = 0;

	for (; str[i]; i++)
	{
		if (str[i] == chars[0])
			counter++;
	}
	return (counter);
}

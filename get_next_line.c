/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: shurtado <shurtado@student.42barcelona.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/03 04:55:46 by shurtado          #+#    #+#             */
/*   Updated: 2024/07/03 06:19:53 by shurtado         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

char	*get_next_line(int fd)
{
	char	*result;
	char	c;
	int		i;

	result = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (read(fd, &c, 1) > 0)
	{
		if (c == '\n')
		{
			result[i] = '\0';
			return (result);
		}
		result[i++] = c;
		if (i >= BUFFER_SIZE)
			return (result);
	}
	return (result);
}

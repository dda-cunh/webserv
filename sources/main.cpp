/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dda-cunh <dda-cunh@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 16:58:00 by dda-cunh          #+#    #+#             */
/*   Updated: 2024/06/24 19:00:48 by dda-cunh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

int	main(int ac, char **av)
{
    (void) ac;
    (void) av;

	int fd = open("Makefile", O_RDONLY);
    std::cout << seek_line((char *)"", (char *)"", 0,fd);
}

# Evaluation Checklist

## Questions

- [ ] The `select()` (or equivalent) should be in the main loop and should check file descriptors for read and write **at the same time**. If not, the grade is 0 and the evaluation process ends now.
- [ ] There should be only one read or one write per client per `select()` (or equivalent). Ask the group to show you the code from the `select()` (or equivalent) to the read and write of a client.
- [ ] Search for all `read`/`recv`/`write`/`send` on a socket and check that, if an error is returned, the client is removed.
- [ ] Search for all `read`/`recv`/`write`/`send` and check if the returned value is correctly checked (checking only -1 or 0 values is not enough; both should be checked).
- [ ] If `errno` is checked after `read`/`recv`/`write`/`send`, the grade is 0 and the evaluation process ends now.
- [ ] Writing or reading **any file descriptor without going through the `select()` (or equivalent)** is strictly forbidden.
- [ ] The project must compile without any re-link issue. If not, use the 'Invalid compilation' flag.

## Configuration

In the configuration file, check whether you can do the following and test the result:

- [ ] Search for the HTTP response status codes list on the internet. During this evaluation, if any status codes are wrong, don't give any related points.
- [ ] Setup multiple servers with different ports.
- [ ] Setup multiple servers with different hostnames (use something like: `curl --resolve example.com:80:127.0.0.1 http://example.com/`).
- [ ] Setup a default error page (try to change the error 404).
- [ ] Limit the client body (use: `curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE" write something shorter or longer than body limit`).
- [ ] Setup routes in a server to different directories.
- [ ] Setup a default file to search for if you ask for a directory.
- [ ] Setup a list of methods accepted for a certain route (e.g., try to delete something with and without permission).

## Basic Checks

Using `telnet`, `curl`, prepared files, demonstrate that the following features work properly:

- [ ] `GET`, `POST`, and `DELETE` requests should work.
- [ ] `UNKNOWN` requests should not result in a crash.
- [ ] For every test, you should receive the appropriate status code.
- [ ] Upload some file to the server and get it back.

## Check CGI

- [ ] The server is working fine using a CGI.
- [ ] The CGI should be run in the correct directory for relative path file access.
- [ ] With the help of the students, check that everything is working properly. Test the CGI with the `GET` and `POST` methods.
- [ ] Test with files containing errors to see if error handling works properly. You can use a script containing an infinite loop or an error; you are free to perform tests within acceptable limits.
- [ ] The server should never crash, and an error should be visible in case of a problem.

## Check with a Browser

- [ ] Use the reference browser of the team. Open the network part of it, and try to connect to the server using it.
- [ ] Look at the request header and response header.
- [ ] It should be compatible with serving a fully static website.
- [ ] Try a wrong URL on the server.
- [ ] Try to list a directory.
- [ ] Try a redirected URL.
- [ ] Try anything you want to.

## Port Issues

- [ ] In the configuration file, setup multiple ports and use different websites. Use the browser to ensure that the configuration works as expected and shows the correct website.
- [ ] In the configuration, try to setup the same port multiple times. It should not work.
- [ ] Launch multiple servers simultaneously with different configurations but with common ports. Does it work? If it does, ask why the server should work if one of the configurations isn't functional. Keep going.

## Siege & Stress Test

- [ ] Use Siege to run some stress tests.
- [ ] Availability should be above 99.5% for a simple `GET` on an empty page with a `siege -b` on that page.
- [ ] Verify there is no memory leak (monitor the process memory usage; it should not increase indefinitely).
- [ ] Check if there are no hanging connections.
- [ ] You should be able to use Siege indefinitely without having to restart the server (check `siege -b`).

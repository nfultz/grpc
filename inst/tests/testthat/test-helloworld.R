library(testthat)
library(methods)
library(grpc)
library(processx)

context('helloworld example')

## start a server in the background
p <- process$new(commandline = "R -e \"demo('helloserver', 'grpc', ask = FALSE)\"")

test_that('server started', {
    expect_true(p$is_alive())
})

## Sys.sleep(5)

test_that('using client', {
    expect_true(any(
        grepl('Hello, Neal',
              capture.output(demo('helloclient', 'grpc', ask = FALSE)))))
})

## kill server
p$kill()

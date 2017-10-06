library(testthat)
library(grpc)
library(parallel)

context('helloworld example')

## start a server in the background
p <- parallel:::mcfork()
parallel:::sendChildStdin(p, "demo('helloserver', 'grpc', ask = FALSE)\n")

test_that('server started', {
    expect_true(inherits(p, 'process'))
})

test_that('using client', {
    expect_true(any(
        grepl('Hello, Neal',
              capture.output(demo('helloclient', 'grpc', ask = FALSE)))))
})

## kill server
parallel:::mckill(p)
